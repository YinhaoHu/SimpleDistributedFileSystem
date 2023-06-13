#include "bufs.h"
#include "core/sdfs_core.h"
#include "snftp/sdfs_snftp.h"
#include "snftp/sdfs_snftp_errno.h"
#include <unistd.h>
#include <time.h>


extern bufs_fs_t __sdfs_bufs; 
 

void sdfs_write(sdfs_sock_fd_t sockfd, sdfs_snftp_arg_t argv[])
{
    int arg1_rcv_inum, arg2_rcv_offset;
    long arg3_rcv_buf_bytes, written_bytes;
    char* arg4_rcv_buf;
    sdfs_int_t rc_send;
    sdfs_snftp_message_t *msg_send;

    arg1_rcv_inum = *(int*)argv[0];
    arg2_rcv_offset = *(int*)argv[1];
    arg3_rcv_buf_bytes = *(long*)argv[2];
    arg4_rcv_buf = (char*)argv[3];

    written_bytes = bufs_write(&__sdfs_bufs, arg1_rcv_inum, arg4_rcv_buf, arg2_rcv_offset,
            arg3_rcv_buf_bytes);
    switch(written_bytes)
    {
        case -1:
            rc_send = SDFS_WRITE_INUM_TYPE;
            break;
        case -2:
            rc_send = SDFS_WRITE_OFFSET;
            break;
        case -3:
            rc_send = SDFS_WRITE_INUM_EXIST;
            break;  
        default:
            sdfs_file_timestamps_update(arg1_rcv_inum, SDFS_UPDATE_FILE_TIMESTAMPS_CURRENT);
            rc_send = written_bytes;
    }
    
    msg_send = sdfs_snftp_message_alloc();
    sdfs_snftp_message_set_pcode(msg_send, 4);
    sdfs_snftp_message_add_arg(msg_send, &rc_send, sizeof(rc_send));
    sdfs_snftp_message_send(sockfd, msg_send);
    sdfs_snftp_message_free(msg_send);
}
