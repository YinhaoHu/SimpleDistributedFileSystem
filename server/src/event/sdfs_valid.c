#include "bufs.h"
#include "core/sdfs_core.h"
#include "snftp/sdfs_snftp.h"



void sdfs_validate(sdfs_sock_fd_t sockfd, sdfs_snftp_arg_t argv[])
{
    sdfs_snftp_message_t *msg_send;
    sdfs_uint_t inum;
    long rc;
    
    inum = *(sdfs_int_t*)argv[0];
    rc = sdfs_file_timestamps_get(inum); 

    msg_send = sdfs_snftp_message_alloc();
    sdfs_snftp_message_set_pcode(msg_send, 7);
    sdfs_snftp_message_add_arg(msg_send, &rc, sizeof(rc));
    sdfs_snftp_message_send(sockfd, msg_send);
    sdfs_snftp_message_free(msg_send);
}

