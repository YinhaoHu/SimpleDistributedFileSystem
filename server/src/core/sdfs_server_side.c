#include "sdfs_tcp_socket.h"
#include "sdfs_thread_pool.h"
#include "bufs.h"
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/socket.h>



bufs_fs_t __sdfs_bufs;

int sdfs_file_timestamps_fd;
long sdfs_file_timestamps[SDFS_MAX_INODES];

static sdfs_sock_fd_t sdfs_server_sock_fd;
static char* sdfs_port;
static char* sdfs_bufs_img_name;
static sdfs_threadpool_t sdfs_thread_pool;


static void sdfs_get_options(int argc, char* argv[]);
static void sdfs_check_essential_files();
static void sdfs_init_bufs(char* img_name);
static void sdfs_init_thread_pool();
static void sdfs_init_inet_socket(char* port);  
static void sdfs_run_server();


int main(int argc, char* argv[])
{  
    sdfs_get_options(argc, argv);
    sdfs_check_essential_files();
    sdfs_init_bufs(sdfs_bufs_img_name);
    sdfs_init_thread_pool();
    sdfs_init_inet_socket(sdfs_port);
    sdfs_file_timestamps_load();
    sdfs_run_server();

    return 0;
}

static void sdfs_get_options(int argc, char* argv[])
{  
    if(argc != 3)
    {
        fprintf(stderr, "Usage:%s <port> <img_name>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    sdfs_port = argv[1];
    sdfs_bufs_img_name = argv[2]; 
}

static void sdfs_check_essential_files()
{ 
    sdfs_return_code_t log_init_rc, file_timestamps_init_rc;

    log_init_rc = sdfs_log_init();
    if(log_init_rc.status != SDFS_RETURN_CODE_STATUS_SUCCESS)
    {
        fprintf(stderr, "%s",log_init_rc.text);
        exit(EXIT_FAILURE);
    }

    file_timestamps_init_rc = sdfs_file_timestamps_init();
    if(file_timestamps_init_rc.status != SDFS_RETURN_CODE_STATUS_SUCCESS)
    {
        fprintf(stderr, "%s",file_timestamps_init_rc.text);
        exit(EXIT_FAILURE);
    }

}

// No check mechanism for simplicity right now.
// Pay attention to use.
static void sdfs_init_bufs(char* img_name)
{ 
    if(bufs_fs_mount(img_name, &__sdfs_bufs) < 0)
    {
        fprintf(stderr, "Mount failure.\n");
        exit(EXIT_FAILURE);
    }
}

static void sdfs_init_thread_pool()
{  
    sdfs_threadpool_init(&sdfs_thread_pool);
}

static void sdfs_init_inet_socket(char* port)
{ 
    sdfs_sock_fd_t res; 
    res = sdfs_tcp_socket_open_listen(port);
    if(res == SDFS_BAD)
    {
        fprintf(stderr, "Error on init internet socket section.\n");
        exit(EXIT_FAILURE);
    }
    sdfs_server_sock_fd = res;
}

static void sdfs_run_server()
{
    sdfs_return_code_t rc;
    sdfs_sock_fd_t client_sock_fd;
     
    while((client_sock_fd = sdfs_tcp_socket_accetpt(sdfs_server_sock_fd, NULL, 0)) != -1)
    {  
        rc = sdfs_threadpool_enqueue(&sdfs_thread_pool, client_sock_fd); 
        sdfs_log_write_rc(&rc);
    }
}
