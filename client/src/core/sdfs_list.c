#include "sdfs.h"
#include "sdfs_inet.h"
#include "snftp/snftp.h"
#include "snftp/snftp_errno.h"

#include <stdlib.h>
#include <string.h>
#include <malloc.h>

sdfs_int_t sdfs_list(sdfs_uint_t dinum, sdfs_stat_t **buffer)
{
    sdfs_size_t buf_size;
    sdfs_int_t rc;
    sdfs_snftp_message_t *msg_send, *msg_recv;

    msg_send = sdfs_snftp_message_alloc();
    sdfs_snftp_message_set_pcode(msg_send, 6);
    sdfs_snftp_message_add_arg(msg_send, &dinum, sizeof(dinum));

    msg_recv = sdfs_reliable_communicate(msg_send);
    sdfs_snftp_message_free(msg_send);
    if(msg_recv == NULL)
        return SDFS_SERVER_CRASH;

    rc = *(sdfs_int_t*)sdfs_snftp_message_get_arg(msg_recv, 0);
    if(rc < 0)
        return rc;
    buf_size = *(sdfs_size_t*)sdfs_snftp_message_get_arg(msg_recv, 1);
    *buffer = (sdfs_stat_t*)malloc(buf_size * sizeof(sdfs_stat_t));
    memcpy(*buffer, sdfs_snftp_message_get_arg(msg_recv, 2), buf_size * sizeof(sdfs_stat_t));
    return buf_size;
}

void sdfs_freelist(sdfs_stat_t **buffer)
{
    free(*buffer);
}
