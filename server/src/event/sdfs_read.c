#include "core/sdfs_core.h"
#include "snftp/sdfs_snftp.h"
#include "snftp/sdfs_snftp_errno.h"
#include "bufs.h"
#include <string.h>
#include <malloc.h>


extern bufs_fs_t __sdfs_bufs;

void sdfs_read(sdfs_sock_fd_t sockfd, sdfs_snftp_arg_t argv[])
{
    int arg1_rcv_inum, arg2_rcv_offset;
    long arg3_rcv_nbytes;
    char *buf;
    sdfs_int_t rc;
    sdfs_snftp_message_t *msg_send;


    arg1_rcv_inum = *(int*)argv[0];
    arg2_rcv_offset = *(int*)argv[1];
    arg3_rcv_nbytes = *(int*)argv[2];
    buf = malloc(arg3_rcv_nbytes);
    rc = (sdfs_int_t)bufs_read(&__sdfs_bufs, arg1_rcv_inum, buf, arg2_rcv_offset, arg3_rcv_nbytes);
    msg_send = sdfs_snftp_message_alloc();
    sdfs_snftp_message_add_arg(msg_send, &rc, sizeof(rc));
    switch (rc)
    {
    case -1:
        rc = SDFS_READ_INUM_TYPE;
        break;
    case -2:
        rc = SDFS_READ_OFFSET;
        break;
    case -3:
        rc = SDFS_READ_INUM_EXIST;
        break;
    default:
        sdfs_snftp_message_add_arg(msg_send, buf, rc);
        break;
    }
    sdfs_snftp_message_send(sockfd, msg_send);
    sdfs_snftp_message_free(msg_send);
    free(buf);
}