#include "sdfs.h"
#include "sdfs_inet.h"
#include "snftp/snftp.h"
#include "snftp/snftp_errno.h"

#include <stdlib.h> 


sdfs_int_t sdfs_stat(sdfs_uint_t inum, sdfs_stat_t* buf)
{
    sdfs_snftp_message_t *msg_send, *msg_recv; 

    msg_send = sdfs_snftp_message_alloc();
    sdfs_snftp_message_set_pcode(msg_send, 1);
    sdfs_snftp_message_add_arg(msg_send, &inum, sizeof(inum));

    msg_recv = sdfs_reliable_communicate(msg_send);
    sdfs_snftp_message_free(msg_send);
    if(msg_recv == NULL) 
        return SDFS_SERVER_CRASH; 

    buf->inum = *((sdfs_uint_t*)sdfs_snftp_message_get_arg(msg_recv, 0));
    buf->type = *((sdfs_uint_t*)sdfs_snftp_message_get_arg(msg_recv, 1));
    buf->size = *((sdfs_uint_t*)sdfs_snftp_message_get_arg(msg_recv, 2));
    sdfs_snftp_message_free(msg_recv);

    return (buf->inum == -1) ? SDFS_STAT_INUM : SDFS_OK;
}

