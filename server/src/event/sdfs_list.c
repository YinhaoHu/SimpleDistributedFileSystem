#include "core/sdfs_core.h"
#include "snftp/sdfs_snftp.h"
#include "snftp/sdfs_snftp_errno.h"
#include "bufs.h"

extern bufs_fs_t __sdfs_bufs; 

void sdfs_list(sdfs_sock_fd_t sockfd, sdfs_snftp_arg_t argv[])
{
    sdfs_int_t rcv_arg_1 ;
    sdfs_int_t rc_send,stats_buf_size;
    bufs_stat_t *stats_buf;
    sdfs_snftp_message_t* msg_send;


    rcv_arg_1 = *(sdfs_int_t*)argv[0];
    stats_buf_size = bufs_list(&__sdfs_bufs, (int)rcv_arg_1, &stats_buf);

    msg_send = sdfs_snftp_message_alloc();
    sdfs_snftp_message_set_pcode(msg_send, 6);
    if(stats_buf_size < 0)
    {
        switch (stats_buf_size)
        {
        case -1:
            rc_send = SDFS_LIST_DINUM_TYPE;
            break;
        case -2:
            rc_send = SDFS_LIST_DINUM_EXIST;
            break;
        }
        sdfs_snftp_message_add_arg(msg_send, &rc_send, sizeof(rc_send));
        sdfs_snftp_message_send(sockfd, msg_send);
        return;
    } 

    rc_send = SDFS_OK;
    sdfs_snftp_message_add_arg(msg_send, &rc_send, sizeof(rc_send));
    sdfs_snftp_message_add_arg(msg_send, &stats_buf_size, sizeof(stats_buf_size));
    sdfs_snftp_message_add_arg(msg_send, stats_buf, stats_buf_size * sizeof(bufs_stat_t));
    
    sdfs_snftp_message_send(sockfd, msg_send);
    
    sdfs_snftp_message_free(msg_send);
    bufs_freelist(&stats_buf);
}