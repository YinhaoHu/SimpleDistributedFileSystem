#include "core/sdfs_core.h"
#include "snftp/sdfs_snftp.h"
#include "snftp/sdfs_snftp_errno.h"
#include "bufs.h"
 
 
extern bufs_fs_t __sdfs_bufs; 

void sdfs_unlink(sdfs_sock_fd_t sockfd, sdfs_snftp_arg_t argv[])
{  
    int inum;
    char* arg_recv_name;
    sdfs_uint_t arg_recv_pinum;
    sdfs_int_t rc; 
    sdfs_snftp_message_t *msg_send; 

    arg_recv_pinum = *(sdfs_uint_t*)argv[0];
    arg_recv_name = (char*)argv[1];

    switch(bufs_unlink(&__sdfs_bufs, arg_recv_pinum, arg_recv_name))
    {
        case -1:
            rc = SDFS_UNLINK_PINUM_EXIST;
            break;
        case -2:
            rc = SDFS_UNLINK_PINUM_TYPE;
            break;
        case -3:
            rc = SDFS_UNLINK_NAME_LONG;
            break;
        case -4:
            rc = SDFS_UNLINK_NOT_EXIST;
            break;
        default:    
            bufs_lookup(&__sdfs_bufs, arg_recv_pinum, arg_recv_name, &inum);
            sdfs_file_timestamps_update(inum, SDFS_UPDATE_FILE_TIMESTAMPS_ZERO); 
    }

    msg_send = sdfs_snftp_message_alloc();
    sdfs_snftp_message_set_pcode(msg_send, 5);
    sdfs_snftp_message_add_arg(msg_send, &rc, sizeof(rc));
    sdfs_snftp_message_send(sockfd, msg_send);
    sdfs_snftp_message_free(msg_send);
}