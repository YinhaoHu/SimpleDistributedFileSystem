#include "sdfs.h"
#include "sdfs_inet.h"
#include "snftp/snftp.h"
#include "snftp/snftp_errno.h"

#include <stdlib.h>
#include <string.h>

sdfs_int_t sdfs_unlink(sdfs_uint_t dinum, char *name)
{
    sdfs_int_t rc;
    size_t name_len = strlen(name);

    if(name == NULL)
        return SDFS_BAD;
    
    if(name_len == 0)
        return SDFS_UNLINK_NAME_EMPTY;

    sdfs_snftp_message_t *msg_send, *msg_recv;

    msg_send = sdfs_snftp_message_alloc();

    sdfs_snftp_message_set_pcode(msg_send, 5);
    sdfs_snftp_message_add_arg(msg_send, &dinum, sizeof(dinum));
    sdfs_snftp_message_add_arg(msg_send, name, name_len+1);

    msg_recv = sdfs_reliable_communicate(msg_send);
    sdfs_snftp_message_free(msg_send);
    if(msg_recv == NULL)
        return SDFS_SERVER_CRASH;
    
    rc = *((sdfs_int_t*)sdfs_snftp_message_get_arg(msg_recv, 0));
    sdfs_snftp_message_free(msg_recv);
    return rc;
}
