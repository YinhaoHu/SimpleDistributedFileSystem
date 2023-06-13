/*!
    Example2: In this example, I'll demonstrate that
    two clients enjoy the server at the same time.
    For simplicity, I will not use the errno macros 
    here and the code liens here are so few.
*/
#include "sdfs.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h> 
#include <errno.h>

#define BUF_SIZE (36)
#define PIPE_MSG_SIZE (128)

static char *host, *service; 
static sdfs_uint_t inum;
 
static void init(int argc, char* argv[]);

int main(int argc, char* argv[])
{
    char write_buf[BUF_SIZE] = "This is SDFS example 2.\n";
    int pipfd[2], fid;

    init(argc, argv);
    sdfs_init(host, service);                                               // SDFS CODE LINE
    inum = sdfs_create(SDFS_ROOT_INUM, "example2", SDFS_REGULAR_FILE);      // SDFS CODE LINE
    sdfs_write(inum, write_buf, 0, BUF_SIZE);                               // SDFS CODE LINE
    
    pipe(pipfd);
    fid = fork();
    if(fid == 0)
    {
        char p_msg[PIPE_MSG_SIZE] = {0};
        char read_buf[BUF_SIZE] = {0};

        sdfs_init(host, service);                                           // SDFS CODE LINE
        sdfs_read(inum, read_buf, 0, BUF_SIZE);                             // SDFS CODE LINE

        close(pipfd[0]);
        snprintf(p_msg, PIPE_MSG_SIZE, "Process %d: read_buf = %s\n", getpid(), read_buf);
        write(pipfd[1], p_msg, PIPE_MSG_SIZE);  

        close(pipfd[1]);
        _exit(0);
    }
    else if(fid > 0)
    {
        ssize_t p_msg_read_bytes = 0;
        char p_msg[PIPE_MSG_SIZE] = {0};

        close(pipfd[1]);

        printf("Process %d: write_buf = %s\n", getpid(), write_buf);
        while((p_msg_read_bytes += read(pipfd[0], p_msg + p_msg_read_bytes, 
            PIPE_MSG_SIZE-p_msg_read_bytes)) != PIPE_MSG_SIZE);
        printf("%s", p_msg);
        close(pipfd[0]);
    }
    else
    {
        perror(strerror(errno));
    }

    return 0;
}


static void init(int argc, char* argv[])
{
    if(argc != 3)
    {
        fprintf(stderr, "Usage: %s <host> <service>\n", argv[0]);
        _exit(1);
    }

    host = argv[1];
    service = argv[2];

    printf("\033[1;37m\tAttention: ensure the bufs_img of the server is new!\033[0m\n");
}