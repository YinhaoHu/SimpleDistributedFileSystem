#ifndef _SDFS_INET_SOCKET_H_
#define _SDFS_INET_SOCKET_H_
 
#include "sdfs_core.h"
#include <sys/socket.h>


//return -1 on error , client socket file descriptor on success.
sdfs_sock_fd_t sdfs_tcp_socket_accetpt(sdfs_sock_fd_t sockfd, struct sockaddr* addr, 
        socklen_t* len);

sdfs_sock_fd_t sdfs_tcp_socket_open_listen(char* port);

#endif