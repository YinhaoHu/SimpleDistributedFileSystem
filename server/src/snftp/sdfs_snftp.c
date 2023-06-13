#include "sdfs_snftp.h"
#include <malloc.h>
#include <string.h>
#include <netdb.h> 
#include <signal.h>
#include <unistd.h>

static void handler_sigpipe(int signum){ };

//return SDFS_BAD if error, otherwise SDFS_OK.
static sdfs_int_t sdfs_snftp_recvn(sdfs_sock_fd_t sockfd, void* buf, sdfs_size_t nbytes);
//return SDFS_BAD if error, otherwise SDFS_OK.
static sdfs_int_t sdfs_snftp_sendn(sdfs_sock_fd_t sockfd, void* buf, sdfs_size_t nbytes);

static sdfs_snftp_arg_t sdfs_snftp_message_new_arg(void* value_ptr, sdfs_size_t value_bytes);

sdfs_snftp_message_t* sdfs_snftp_message_alloc()
{
    sdfs_size_t type_size;
    sdfs_snftp_message_t * new_message;

    type_size = sizeof(sdfs_snftp_message_t);
    new_message = (sdfs_snftp_message_t*)malloc(type_size);
    memset(new_message, 0, type_size);

    new_message->argc = 0;

    return new_message;
}
 
void sdfs_snftp_message_free(sdfs_snftp_message_t* message_ptr)
{
    sdfs_size_t argc = message_ptr->argc;

    for(sdfs_size_t idx = 0; idx < argc; ++idx) 
        free(message_ptr->argv[idx]); 
    
    free(message_ptr);
}
 
void sdfs_snftp_message_set_pcode(sdfs_snftp_message_t* message, sdfs_uint_t code)
{
    message->procedure_code = code;
}

sdfs_uint_t sdfs_snftp_message_get_pcode(sdfs_snftp_message_t* message)
{
    return message->procedure_code;
}


sdfs_snftp_arg_t sdfs_snftp_message_get_arg(const sdfs_snftp_message_t* message, sdfs_uint_t arg_idx)
{
    return (message->argv[arg_idx]);
}

void sdfs_snftp_message_add_arg(sdfs_snftp_message_t* message,
    void* arg, sdfs_size_t arg_bytes)
{
    sdfs_uint_t idx = message->argc;

    message->arg_bytes[idx] = arg_bytes;
    message->argv[idx] = sdfs_snftp_message_new_arg(arg, arg_bytes);
    (message->argc)++;
}


sdfs_int_t sdfs_snftp_message_send(sdfs_sock_fd_t sock_fd, sdfs_snftp_message_t* message)
{
    //For simplicity, we define this function only sutable on windows and Linux.
    //So the byte-order is both big-endian.
    //message bytes = header bytes + args bytes sum.
    char *buf;
    sdfs_int_t ret = SDFS_OK;
    sdfs_size_t m_bytes, unit_off_end;
    const sdfs_size_t unit_bytes = sizeof(sdfs_uint_t); 

    m_bytes = SDFS_SNFTP_HEADER_BYTES;
    for(sdfs_size_t i = 0; i < message->argc; ++i)
        m_bytes += message->arg_bytes[i];
    buf = (char*)malloc(m_bytes);
    memset(buf, 0, m_bytes);
    
    // Put message->procedure_code and message->argc
    memcpy(buf + 0 * unit_bytes ,&(message->procedure_code) ,unit_bytes); 
    memcpy(buf + 1 * unit_bytes ,&(message->argc) ,unit_bytes);

    unit_off_end = 2 + message->argc;
    // Put message->argv_bytes.
    for(sdfs_size_t unit_off = 2; unit_off < unit_off_end; unit_off++) 
        memcpy(buf + unit_off * unit_bytes, &(message->arg_bytes[unit_off-2]), unit_bytes); 
    
    // Put message->argv
    for(sdfs_size_t argv_off = 0, accu = 0; argv_off < message->argc; ++argv_off)
    {
        sdfs_size_t arg_bytes = message->arg_bytes[argv_off];
        memcpy(buf + SDFS_SNFTP_HEADER_BYTES + accu, message->argv[argv_off], arg_bytes);
        accu += arg_bytes;
    }

    ret = sdfs_snftp_sendn(sock_fd, buf, m_bytes);
    free(buf);
    
    return ret;
}

sdfs_snftp_message_t* sdfs_snftp_message_recv(sdfs_sock_fd_t sock_fd)
{
    char header_buf[SDFS_SNFTP_HEADER_BYTES] = {0}, *args_buf = NULL;
    sdfs_size_t unit_bytes, unit_off, args_bytes, args_accu_bytes;
    sdfs_snftp_message_t* msg = NULL;
    
    unit_bytes = sizeof(sdfs_uint_t);
    unit_off = 0;
    msg = (sdfs_snftp_message_t*)malloc(sizeof(sdfs_snftp_message_t));  
    if(sdfs_snftp_recvn(sock_fd, header_buf, SDFS_SNFTP_HEADER_BYTES) == SDFS_BAD)
        goto BAD_RETURN;

    memcpy(&msg->procedure_code, header_buf + (unit_off++) * unit_bytes, unit_bytes); 
    memcpy(&msg->argc, header_buf + (unit_off++) * unit_bytes, unit_bytes); 
    for(args_bytes = 0; unit_off < (msg->argc + 2); ++unit_off) 
    {
        memcpy(&(msg->arg_bytes[unit_off-2]), header_buf+unit_off*unit_bytes, unit_bytes); 
        args_bytes += msg->arg_bytes[unit_off-2];
    }

    args_buf = (char*)malloc(args_bytes);
    if(sdfs_snftp_recvn(sock_fd, args_buf, args_bytes) == SDFS_BAD)
        goto BAD_RETURN;

    args_accu_bytes = 0;
    for(sdfs_size_t arg_id = 0; arg_id < msg->argc; ++arg_id)
    {
        msg->argv[arg_id] = malloc(msg->arg_bytes[arg_id]);
        memcpy(msg->argv[arg_id], args_buf+args_accu_bytes, msg->arg_bytes[arg_id]);
        args_accu_bytes += msg->arg_bytes[arg_id];
    }
    free(args_buf);
    return msg;

BAD_RETURN: 
    if(args_buf != NULL)
        free(args_buf);
    if(msg != NULL)
        free(msg);

    return NULL;
}

static sdfs_int_t sdfs_snftp_recvn(sdfs_sock_fd_t sockfd, void* buf, sdfs_size_t nbytes)
{
    sdfs_size_t recvn_bytes = 0, rc;

    signal(SIGPIPE, handler_sigpipe);
    while(recvn_bytes != nbytes)
    {  
        rc = recv(sockfd, buf + recvn_bytes, nbytes-recvn_bytes, 0); 
        if(rc <= 0) 
            break; 
        recvn_bytes += rc;
    } 
    signal(SIGPIPE, SIG_DFL);
    return (recvn_bytes == nbytes) ? SDFS_OK : SDFS_BAD;
}

static sdfs_int_t sdfs_snftp_sendn(sdfs_sock_fd_t sockfd, void* buf, sdfs_size_t nbytes)
{
    sdfs_size_t sendn_bytes = 0, rc;
 
    signal(SIGPIPE, handler_sigpipe);
    while (sendn_bytes != nbytes)
    { 
        rc = send(sockfd, buf + sendn_bytes, nbytes-sendn_bytes, 0);  
        if(rc == -1) 
            break;
        sendn_bytes += rc;
    } 
    signal(SIGPIPE, SIG_DFL);
    return (sendn_bytes == nbytes) ? SDFS_OK : SDFS_BAD;
}


static sdfs_snftp_arg_t sdfs_snftp_message_new_arg(void* value_ptr, sdfs_size_t value_bytes)
{
    sdfs_snftp_arg_t ret = malloc(value_bytes);
    memcpy(ret, value_ptr, value_bytes);
    return ret;
}