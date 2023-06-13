#ifndef _SDFS_EVENT_H_
#define _SDFS_EVENT_H_

#include "core/sdfs_core.h"
#include "snftp/sdfs_snftp.h"

#define SDFS_EVENT_WAITING_TIMEOUT (30)

#define SDFS_EVENT_NUMS (8)

#define SDFS_EVENT_PCODE_ISVALID(pcode) ((pcode < SDFS_EVENT_NUMS) && (pcode >= 0))

typedef void(*sdfs_event_function_t)(sdfs_sock_fd_t,sdfs_snftp_arg_t[]);

// get the message procedure code and select one procedure to
// execute.
void sdfs_event_response(sdfs_sock_fd_t sockfd, sdfs_snftp_message_t* msg);

// Waiting for client's request. Return dynamically allocated
// message if received, NULL if timeout.
sdfs_snftp_message_t* sdfs_event_waiting(sdfs_sock_fd_t sockfd);

void sdfs_lookup(sdfs_sock_fd_t sockfd, sdfs_snftp_arg_t argv[]);

void sdfs_stat(sdfs_sock_fd_t sockfd, sdfs_snftp_arg_t argv[]);

void sdfs_create(sdfs_sock_fd_t sockfd, sdfs_snftp_arg_t argv[]);

void sdfs_write(sdfs_sock_fd_t sockfd, sdfs_snftp_arg_t argv[]);

void sdfs_read(sdfs_sock_fd_t sockfd, sdfs_snftp_arg_t argv[]);

void sdfs_unlink(sdfs_sock_fd_t sockfd, sdfs_snftp_arg_t argv[]);

void sdfs_list(sdfs_sock_fd_t sockfd, sdfs_snftp_arg_t argv[]);

void sdfs_validate(sdfs_sock_fd_t sockfd, sdfs_snftp_arg_t argv[]);

#endif