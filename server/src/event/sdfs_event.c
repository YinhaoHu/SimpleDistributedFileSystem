#include "sdfs_event.h"
#include "core/sdfs_core.h"
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/time.h> 
#include <string.h>
#include <stdio.h>

 
static sdfs_event_function_t sdfs_event_functions[SDFS_EVENT_NUMS] = 
{
    sdfs_lookup , sdfs_stat,
    sdfs_create , sdfs_read,
    sdfs_write  , sdfs_unlink,
    sdfs_list   , sdfs_validate
};


void sdfs_event_response(sdfs_sock_fd_t sockfd, sdfs_snftp_message_t* msg)
{
    sdfs_uint_t pcode = sdfs_snftp_message_get_pcode(msg);
    char *log_msg = malloc(SDFS_LOG_MSG_LEN);

    memset(log_msg, 0, SDFS_LOG_MSG_LEN);
    if( SDFS_EVENT_PCODE_ISVALID(pcode) ) 
    {
        (sdfs_event_functions[pcode])(sockfd, msg->argv); 
        snprintf(log_msg, SDFS_LOG_MSG_LEN,"Response procedure code:%d\n",
            msg->procedure_code);
    }
    else
    {
        snprintf(log_msg, SDFS_LOG_MSG_LEN,"Invalid procedure code:%d\n",
            msg->procedure_code);
    }
    sdfs_log_write_msg(log_msg);
    free(log_msg);
}

 
sdfs_snftp_message_t* sdfs_event_waiting(sdfs_sock_fd_t sockfd)
{
    fd_set fds; 
    char check_close_buf[4];
    sdfs_snftp_message_t* msg = NULL;
    struct timeval timeout; 

    timeout.tv_sec = SDFS_EVENT_WAITING_TIMEOUT;
    timeout.tv_usec = 0;

    FD_ZERO(&fds);
    FD_SET(sockfd, &fds);
 
    switch ( select(sockfd+1, &fds, NULL, NULL, &timeout) )
    {
    case -1: 
        fprintf(stdout, "sdfs_event_wating: error in select.\n");
        break;
    case 0:
        msg = NULL;
        break;
    default:
        if(recv(sockfd, check_close_buf, 4, MSG_PEEK) != 0) 
            msg = sdfs_snftp_message_recv(sockfd); 
    }       
    
    return msg;
}