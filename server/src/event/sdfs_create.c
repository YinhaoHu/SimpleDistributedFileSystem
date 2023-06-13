#include "core/sdfs_core.h"
#include "snftp/sdfs_snftp.h"
#include "snftp/sdfs_snftp_errno.h"
#include "bufs.h"
#include <string.h>


extern bufs_fs_t __sdfs_bufs; 

void sdfs_create(sdfs_sock_fd_t sockfd, sdfs_snftp_arg_t argv[])
{
    int bufs_rc, arg_pinum, arg_type;
    char *arg_name; 
    sdfs_int_t send_rc;
    sdfs_snftp_message_t* msg_send = sdfs_snftp_message_alloc();

    arg_pinum = *(int*)argv[0];
    arg_name = (char*)argv[1];
    arg_type = *(int*)argv[2];
 

    bufs_rc = bufs_create(&__sdfs_bufs, arg_pinum, arg_name, arg_type);

    // Returned value -5 case was checked on the client-side.
    switch (bufs_rc)
    {
    case -1:
        send_rc = SDFS_CREATE_SPACE;
        break;
    case -2:
        send_rc = SDFS_CREATE_INUM_NDIR;
        break;
    case -3:
        send_rc = SDFS_CREATE_INUM_EXIST;
        break;
    case -4:
        send_rc = SDFS_CREATE_LONG_NAME;
        break;
    case -6:
        send_rc = SDFS_CREATE_NAME_EXIST;
        break;
    default:
        send_rc = bufs_rc;
    }

    sdfs_snftp_message_add_arg(msg_send, &send_rc, sizeof(send_rc));
    sdfs_snftp_message_send(sockfd, msg_send);
    sdfs_snftp_message_free(msg_send);
}