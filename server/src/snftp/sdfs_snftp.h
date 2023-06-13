#ifndef _SDFS_SNFTP_H_
#define _SDFS_SNFTP_H_

#include "core/sdfs_core.h"

#define SDFS_SNFTP_MAX_ARGS (8)
#define SDFS_SNFTP_HEADER_BYTES (64)

#define SDFS_SOCK_RECV_TIMEOUT (3)
#define SDFS_SOCK_SEND_TIMEOUT (3)

typedef void* sdfs_snftp_arg_t;

//Args must be allocated in the heap.
typedef struct _sdfs_snftp_message{
    sdfs_uint_t procedure_code; 
    sdfs_size_t argc;
    sdfs_size_t arg_bytes[SDFS_SNFTP_MAX_ARGS];
    sdfs_snftp_arg_t argv[SDFS_SNFTP_MAX_ARGS];
}sdfs_snftp_message_t;

//Just allocate sdfs_snftp_message_t, the memory of 
//agrs should be allocated on the heap mannually.
sdfs_snftp_message_t* sdfs_snftp_message_alloc();

//free the heap memory of args and sdfs_snftp_message_t.
void sdfs_snftp_message_free(sdfs_snftp_message_t* message_ptr);


// This function only ensures one send is reliable.
// return SDFS_OK or SDFS_BAD
sdfs_int_t sdfs_snftp_message_send(sdfs_sock_fd_t sock_fd, sdfs_snftp_message_t* message);

// return dynamically allocated message on success, NULL on failure.
// The user takes the responsibility to free them.
// This function only ensures one recv is reliable.
sdfs_snftp_message_t* sdfs_snftp_message_recv(sdfs_sock_fd_t sock_fd);

// Set procedure code.
void sdfs_snftp_message_set_pcode(sdfs_snftp_message_t* message, sdfs_uint_t code);

// Get procedure code.
sdfs_uint_t sdfs_snftp_message_get_pcode(sdfs_snftp_message_t* message);

// Add one arg to message.
void sdfs_snftp_message_add_arg(sdfs_snftp_message_t* message,void* arg, sdfs_size_t arg_bytes);


// Get the arg, no border check.
sdfs_snftp_arg_t sdfs_snftp_message_get_arg(const sdfs_snftp_message_t* message, sdfs_uint_t arg_idx);



#endif