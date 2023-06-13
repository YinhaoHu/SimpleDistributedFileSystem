#include "sdfs.h"
#include "sdfs_inet.h"
#include "snftp/snftp.h"
#include "snftp/snftp_errno.h"

#include <stdlib.h>

sdfs_int_t sdfs_write(sdfs_uint_t inum, char *buf, sdfs_size_t offset, sdfs_size_t nbytes)
{
    // Implement this function and then write read function.
    sdfs_int_t rc;
    sdfs_snftp_message_t *msg_send, *msg_recv;

    msg_send = sdfs_snftp_message_alloc();
    sdfs_snftp_message_set_pcode(msg_send, 4);
    sdfs_snftp_message_add_arg(msg_send, &inum, sizeof(inum));
    sdfs_snftp_message_add_arg(msg_send, &offset, sizeof(offset));
    sdfs_snftp_message_add_arg(msg_send, &nbytes, sizeof(nbytes));
    sdfs_snftp_message_add_arg(msg_send, buf, nbytes);

    msg_recv = sdfs_reliable_communicate(msg_send);
    sdfs_snftp_message_free(msg_send);
    if(msg_recv == NULL)
        return SDFS_SERVER_CRASH;

    rc = *(sdfs_int_t*)sdfs_snftp_message_get_arg(msg_recv, 0);
    sdfs_snftp_message_free(msg_recv);
    return rc;
}