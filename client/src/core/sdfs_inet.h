#ifndef _SDFS_INET_H_
#define _SDFS_INET_H_

#include "sdfs.h"
#include "snftp/snftp.h"


// Retrun NULL if bad communicate, otherwise, dynamically allocated 
// message. Remember to free it correctly.
sdfs_snftp_message_t* sdfs_reliable_communicate(sdfs_snftp_message_t* msg_buf);

// Return SDFS_OK if running, SDFS_BAD if not.
sdfs_int_t sdfs_server_is_running();

// To establish a TCP connection to the server.
sdfs_sock_fd_t sdfs_connect(char *host, char* service);


#endif