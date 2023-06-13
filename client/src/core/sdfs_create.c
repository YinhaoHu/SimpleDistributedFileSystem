#include "sdfs.h"
#include "sdfs_inet.h"
#include "snftp/snftp.h"
#include "snftp/snftp_errno.h"

#include <string.h>

sdfs_int_t sdfs_create(sdfs_uint_t pinum, char *name, sdfs_uint_t type)
{
    if(name == NULL)
        return SDFS_BAD;

    if(strlen(name) == 0)
        return SDFS_CREATE_NAME_EMPTY;

    if( (type != SDFS_REGULAR_FILE) && (type != SDFS_DIRECOTRY))
        return SDFS_CREATE_TYPE;

    sdfs_snftp_message_t *msg_send, *msg_recv;
    sdfs_int_t rc = SDFS_OK;

    msg_send = sdfs_snftp_message_alloc();
    sdfs_snftp_message_set_pcode(msg_send, 2);
    sdfs_snftp_message_add_arg(msg_send, &pinum, sizeof(pinum));
    sdfs_snftp_message_add_arg(msg_send, name, strlen(name)+1);
    sdfs_snftp_message_add_arg(msg_send, &type, sizeof(type));

    msg_recv = sdfs_reliable_communicate(msg_send);
    sdfs_snftp_message_free(msg_send);
    if(msg_recv == NULL)
        return SDFS_SERVER_CRASH;

    rc = *((sdfs_int_t*)sdfs_snftp_message_get_arg(msg_recv, 0));
    sdfs_snftp_message_free(msg_recv);
    
    return rc;
}


