#define _GNU_SOURCE

#include "sdfs_inet.h"
#include <stdlib.h>
#include <string.h>
#include <netdb.h> 
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <stdio.h>

static sdfs_sock_fd_t global_sockfd;
static char* server_hostname;
static char* server_service;

sdfs_sock_fd_t sdfs_connect(char *host, char *service)
{
    sdfs_sock_fd_t sockfd;
    struct addrinfo hints, *result, *p;
    struct timeval send_timeout, recv_timeout;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    server_hostname = host;
    server_service = service;

    send_timeout.tv_sec = SDFS_SOCK_SEND_TIMEOUT;
    send_timeout.tv_usec = 0;
    recv_timeout.tv_sec = SDFS_SOCK_RECV_TIMEOUT;
    recv_timeout.tv_usec = 0;

    getaddrinfo(server_hostname, server_service, &hints, &result);
    global_sockfd = SDFS_BAD;
    for(p = result; p != NULL; p = p->ai_next)
    {
        sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol); 
        if(sockfd == -1)
            continue;

        if((setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, &send_timeout, 
                sizeof(send_timeout)) == -1)
         ||(setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &recv_timeout,
                sizeof(recv_timeout)) == -1))
            {
                close(sockfd);
                freeaddrinfo(result);
                return SDFS_BAD;
            }
        if(connect(sockfd, p->ai_addr, p->ai_addrlen) == 0)
            break;
        close(sockfd);
    }

    freeaddrinfo(result);

    return (p == NULL) ? SDFS_BAD : (global_sockfd = sockfd);
}


sdfs_snftp_message_t* sdfs_reliable_communicate(sdfs_snftp_message_t* msg_to_sent)
{
    /*
        The network congestion and server crash both can induce the send failure.
        Sudden server crash can introduce the recv failure.\
        Retry is still helpful because of the reasons mentioned above
        and we don't want to recv for a long time because of the server crash.
        If server can not response on time which is set by our means, we just
        return the invoking application an error.
    */
    sdfs_int_t rc;
    sdfs_uint_t retry;
    sdfs_snftp_message_t* msg_to_recv = NULL;

    for(retry = 0; retry < SDFS_TRANSPORT_RETRY_COUNT; ++retry)
    { 
        if(global_sockfd == SDFS_BAD)
            sdfs_connect(server_hostname, server_service);
 
        rc = sdfs_snftp_message_send(global_sockfd, msg_to_sent); 
        if(rc == SDFS_BAD)
            continue;
            
        if(sdfs_server_is_running() != SDFS_OK)
        { 
            sdfs_connect(server_hostname, server_service);
            continue;
        }

        msg_to_recv = sdfs_snftp_message_recv(global_sockfd);
 
        if(msg_to_recv != NULL) 
            break;
        sleep(SDFS_TRANSPORT_RETRY_INTERVAL);
    }

    return msg_to_recv;
}


sdfs_int_t sdfs_server_is_running()
{
    ssize_t recv_bytes;
    char check_buf[4];

    recv_bytes = recv(global_sockfd, check_buf, 4, MSG_PEEK); 
    return ( recv_bytes != 0 && global_sockfd != SDFS_BAD) ? SDFS_OK : SDFS_BAD;
}