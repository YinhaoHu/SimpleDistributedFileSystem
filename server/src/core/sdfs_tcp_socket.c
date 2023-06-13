#define _GNU_SOURCE

#include "sdfs_tcp_socket.h"
#include "../snftp/sdfs_snftp.h"

#include <unistd.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h> 
#include <netdb.h>
#include <string.h>


sdfs_sock_fd_t sdfs_tcp_socket_accetpt(sdfs_sock_fd_t sockfd,struct sockaddr* addr, socklen_t* len)
{ 
    sdfs_sock_fd_t ret;
    socklen_t opt_size;
    struct timeval notimeout, oldtimeout;

    notimeout.tv_sec = 0;
    notimeout.tv_usec = 0;
    opt_size = sizeof(struct timeval);
    if(getsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &oldtimeout, &opt_size) < 0)
        return -1;
    if(setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &notimeout, sizeof(notimeout)) < 0)
        return -1;

    while((ret = accept(sockfd, addr, len)) == -1);

    if(setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &oldtimeout, sizeof(oldtimeout)) < 0)
        return -1;

    return ret;
};

// Open a IPV4 listen stream socket file descriptor with given port. 
sdfs_sock_fd_t sdfs_tcp_socket_open_listen(char* port)
{
    // Set socket addr info.
    // Open a socket
    // Set option
    // bind
    // listen    
    sdfs_sock_fd_t sock_fd; 
    struct addrinfo hints, *result, *p;
    struct timeval send_timeout, recv_timeout; 
    int sock_reuse_addr_opt_val; 

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = 0;
    hints.ai_flags = AI_PASSIVE;

    sock_reuse_addr_opt_val = 1;
    recv_timeout.tv_sec = SDFS_SOCK_RECV_TIMEOUT;
    recv_timeout.tv_usec = 0;
    send_timeout.tv_sec = SDFS_SOCK_SEND_TIMEOUT;
    send_timeout.tv_usec = 0;

    getaddrinfo(NULL, port, &hints, &result);
    for(p = result; p != NULL; p = p->ai_next)
    {
        sock_fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if(sock_fd == -1)
            continue;;
         
        if((setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR,&sock_reuse_addr_opt_val,
                sizeof(sock_reuse_addr_opt_val))==-1)
         ||(setsockopt(sock_fd, SOL_SOCKET, SO_RCVTIMEO, &recv_timeout, 
                sizeof(recv_timeout)) == -1)
         ||(setsockopt(sock_fd, SOL_SOCKET, SO_SNDTIMEO, &send_timeout, 
                sizeof(send_timeout)) ==-1))
            goto BAD_RETURN;
     

        if(bind(sock_fd, p->ai_addr, p->ai_addrlen) == 0)
            break;
        close(sock_fd);
    }

    if(listen(sock_fd, SOMAXCONN) == -1)
        goto BAD_RETURN;
 
    freeaddrinfo(result);
    return (p == NULL) ? SDFS_BAD : sock_fd;

BAD_RETURN:
    freeaddrinfo(result);
    close(sock_fd);
    return SDFS_BAD;
}