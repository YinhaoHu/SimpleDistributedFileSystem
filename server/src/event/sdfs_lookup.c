#include "core/sdfs_core.h"
#include "snftp/sdfs_snftp_errno.h"
#include "snftp/sdfs_snftp.h"
#include "event/sdfs_event.h"
#include "bufs.h" 


extern bufs_fs_t __sdfs_bufs; 

void sdfs_lookup(sdfs_sock_fd_t sockfd, sdfs_snftp_arg_t argv[])
{  
    int inum;
    sdfs_snftp_message_t* msg = sdfs_snftp_message_alloc();
    sdfs_int_t arg_val;
    sdfs_size_t arg_size = sizeof(sdfs_int_t); 

    sdfs_snftp_message_set_pcode(msg, 0); 
    
    switch (bufs_lookup(&__sdfs_bufs, *((int*)argv[0]), (char*)(argv[1]), &inum))
    {
    case 1:
        arg_val = inum;
        break;
    case 0:
        arg_val = SDFS_LOOKUP_NOT_FOUND;
        break;
    case -1:
        arg_val = SDFS_LOOKUP_PINUM_DIR;
        break;
    case -2:
        arg_val = SDFS_LOOKUP_PINUM_EXIST;
        break;
    default:
        break;
    }
    
    sdfs_snftp_message_add_arg(msg,&arg_val,arg_size);
    sdfs_snftp_message_send(sockfd, msg);
    sdfs_snftp_message_free(msg);
}