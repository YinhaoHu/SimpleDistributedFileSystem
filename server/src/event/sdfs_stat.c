#include "core/sdfs_core.h"
#include "snftp/sdfs_snftp_errno.h"
#include "snftp/sdfs_snftp.h"
#include "event/sdfs_event.h"
#include "bufs.h" 
#include <string.h>
#include <stdio.h>

extern bufs_fs_t __sdfs_bufs; 

void sdfs_stat(sdfs_sock_fd_t sockfd, sdfs_snftp_arg_t argv[])
{
    int inum = -1, type = -1, size = -1,rc;
    bufs_stat_t stat_buf;
    sdfs_snftp_message_t* msg_send;

    memset(&stat_buf, 0, sizeof(bufs_stat_t));
    rc = bufs_stat(&__sdfs_bufs, *((int*)(argv[0])),&stat_buf);
    if(rc >= 0) 
    {
        inum = stat_buf.inum;
        type = stat_buf.type;
        size = stat_buf.size;
    } 

    msg_send = sdfs_snftp_message_alloc();
    sdfs_snftp_message_set_pcode(msg_send, 1);
    sdfs_snftp_message_add_arg(msg_send, &(inum), sizeof(inum));
    sdfs_snftp_message_add_arg(msg_send, &(type), sizeof(type));
    sdfs_snftp_message_add_arg(msg_send, &(size), sizeof(size));

    sdfs_snftp_message_send(sockfd, msg_send);
}