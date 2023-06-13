#include "sdfs_inet.h"
#include "sdfs.h"
#include "snftp/snftp_errno.h"
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <stdio.h> 
#include <malloc.h> 
#include <errno.h>
/*
            Buffer Read Algorithm

     Check whether there is a local buffer file to read.
        
        If so, we get the time stamp of the file by sending
        the validate request to check the cache consistency,
        and send the read() request depending on the consis-
        tency.
        
        Else, we just send the read() request.

*/

// Free the return pointer.
static char* sdfs_buffer_file_name(sdfs_uint_t inum);
// Return 1 on exist, 0 on not. 
static sdfs_int_t sdfs_buffer_file_exist(sdfs_uint_t inum);
static ssize_t sdfs_buffer_file_write(sdfs_uint_t inum, char *buf, sdfs_size_t offset, sdfs_size_t nbytes);
static ssize_t sdfs_buffer_file_read(sdfs_uint_t inum, char *buf, sdfs_size_t offset, sdfs_size_t nbytes);

// Return 1 if old, 0 if new, -1 on not existing inum. Server crash macro will be returned
// possibly.
static sdfs_int_t sdfs_buffer_file_is_old(sdfs_uint_t inum);


sdfs_int_t sdfs_read(sdfs_uint_t inum, char *buf, sdfs_size_t offset, sdfs_size_t nbytes)
{
    sdfs_int_t is_old_rc, rc, recv_rc, buf_exist_rc;
    sdfs_snftp_message_t *msg_send, *msg_recv;

    rc = 0;
    buf_exist_rc = sdfs_buffer_file_exist(inum); 
    if(buf_exist_rc == 1)
    {  
        is_old_rc = sdfs_buffer_file_is_old(inum);
        switch (is_old_rc)
        {
        case 1:
            rc = sdfs_buffer_file_read(inum, buf, offset, nbytes); 
            goto RETURN; 
        case 0:
            break;
        case -1: 
            rc = SDFS_READ_INUM_EXIST;
            goto RETURN;
        case SDFS_SERVER_CRASH:
            rc = SDFS_SERVER_CRASH; 
            goto RETURN; 
        }
    }
    
    msg_send = sdfs_snftp_message_alloc();
    sdfs_snftp_message_set_pcode(msg_send, 3);
    sdfs_snftp_message_add_arg(msg_send, &inum, sizeof(inum));
    sdfs_snftp_message_add_arg(msg_send, &offset, sizeof(offset));
    sdfs_snftp_message_add_arg(msg_send, &nbytes, sizeof(nbytes));
    msg_recv = sdfs_reliable_communicate(msg_send);
    sdfs_snftp_message_free(msg_send);
    if(msg_recv == NULL)
    {
        rc = SDFS_SERVER_CRASH;
        goto RETURN;
    }

    recv_rc = *(sdfs_int_t*)sdfs_snftp_message_get_arg(msg_recv, 0);
    if(recv_rc >= 0)
    {    
        if(buf_exist_rc == 0)
        {
            char *name= sdfs_buffer_file_name(inum); 
            close(creat(name, 0777)); 
            free(name); 
        } 
        memcpy(buf, sdfs_snftp_message_get_arg(msg_recv, 1), recv_rc);
        sdfs_buffer_file_write(inum, buf, offset, nbytes);
    }
    rc = recv_rc;
    sdfs_snftp_message_free(msg_recv);
RETURN:
    return rc;
}


static char* sdfs_buffer_file_name(sdfs_uint_t inum)
{ 
    char *name = malloc(64);
    
    memset(name, 0, 64);
    sprintf(name,"%s/%d", SDFS_BUFFER_DIR_NAME, inum);

    return name;
}


static sdfs_int_t sdfs_buffer_file_exist(sdfs_uint_t inum)
{
    sdfs_int_t rc = 1;
    char* inum_str = sdfs_buffer_file_name(inum); 
    
    if(access(inum_str, F_OK) != 0) 
        rc = 0;  
 
    free(inum_str);
    return rc;
}


static ssize_t sdfs_buffer_file_write(sdfs_uint_t inum, char *buf, 
    sdfs_size_t offset, sdfs_size_t nbytes)
{
    char* inum_str = sdfs_buffer_file_name(inum);
    int inum_fd= open(inum_str, 0666);
    ssize_t rc;
    
    lseek(inum_fd, offset, SEEK_SET);
    rc = write(inum_fd, buf, nbytes);
    fsync(inum_fd);
    close(inum_fd);

    free(inum_str);
    return rc;
}


static ssize_t sdfs_buffer_file_read(sdfs_uint_t inum, char *buf, 
    sdfs_size_t offset, sdfs_size_t nbytes)
{
    char *inum_str= sdfs_buffer_file_name(inum); 
    int inum_fd= open(inum_str, 0666);
    ssize_t rc;
    
    lseek(inum_fd, offset, SEEK_SET);
    rc = read(inum_fd, buf, nbytes); 
    close(inum_fd);

    free(inum_str);
    return rc;
}


static sdfs_int_t sdfs_buffer_file_is_old(sdfs_uint_t inum)
{
    char *inum_name; 
    struct stat stat_buf;
    sdfs_snftp_message_t *msg_send, *msg_recv;
    long timestamp;

    inum_name= sdfs_buffer_file_name(inum);
    msg_send = sdfs_snftp_message_alloc();
    sdfs_snftp_message_set_pcode(msg_send, 7);
    sdfs_snftp_message_add_arg(msg_send, &inum, sizeof(inum));
    msg_recv = sdfs_reliable_communicate(msg_send);
    sdfs_snftp_message_free(msg_send);
    if(msg_recv == NULL)
    { 
        free(inum_name);
        return SDFS_SERVER_CRASH;
    }
    
    stat(inum_name, &stat_buf);
    timestamp = *(long*)sdfs_snftp_message_get_arg(msg_recv, 0); 
    sdfs_snftp_message_free(msg_recv);
    if(timestamp == 0)
    {
        free(inum_name); 
        return -1;
    } 
    return (timestamp <= stat_buf.st_mtime);
}