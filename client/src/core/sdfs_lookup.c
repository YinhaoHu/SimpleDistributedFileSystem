#include "sdfs.h"
#include "sdfs_inet.h"
#include "snftp/snftp.h"
#include "snftp/snftp_errno.h"

#include <stdlib.h>
#include <string.h>


sdfs_int_t sdfs_lookup(sdfs_uint_t pinum, char* name)
{
    sdfs_int_t ret;
    sdfs_snftp_message_t* msg_send, *msg_recv;
    sdfs_size_t arg1_size = sizeof(sdfs_uint_t), arg2_size = strlen(name)+1;

    msg_send = sdfs_snftp_message_alloc();
    sdfs_snftp_message_set_pcode(msg_send, 0);
    sdfs_snftp_message_add_arg(msg_send, &pinum, arg1_size);
    sdfs_snftp_message_add_arg(msg_send, name, arg2_size);

    msg_recv = sdfs_reliable_communicate(msg_send);
    sdfs_snftp_message_free(msg_send);

    if(msg_recv == NULL) 
        return SDFS_SERVER_CRASH; 
    
    ret = *((sdfs_int_t*)(msg_recv->argv[0]));
    sdfs_snftp_message_free(msg_recv);
    return ret;
}

