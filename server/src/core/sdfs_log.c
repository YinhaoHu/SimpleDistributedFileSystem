#include "sdfs_config.h"
#include "sdfs_core.h"
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <malloc.h>
#include <string.h>

#define SDFS_LOG_ENTRY_STRLEN (128)

static int sdfs_log_fd;

sdfs_return_code_t sdfs_log_init()
{
    sdfs_return_code_t rc = SDFS_RETURN_CODE_INITIALIZER;

    sdfs_log_fd = open(SDFS_SERVER_LOG_FILE_NAME, O_CREAT | O_RDWR, 0666);

    if(sdfs_log_fd < 0)
    {
        rc.status = SDFS_RETURN_CODE_STATUS_FAILURE;
        perror(strerror(errno));
        rc.text = "Log initializer failure.\n";
    }
    return rc;
}

void sdfs_log_write_rc(sdfs_return_code_t* rc)
{
    char *msg = malloc(SDFS_LOG_ENTRY_STRLEN);
    memset(msg, 0, SDFS_LOG_ENTRY_STRLEN);
    snprintf(msg, SDFS_LOG_ENTRY_STRLEN,"%d %d %s\n",rc->status,  rc->code, rc->text);
    write(sdfs_log_fd, msg, strlen(msg)); 
    fsync(sdfs_log_fd);
    free(msg);
}


void sdfs_log_write_msg(char* msg)
{
    write(sdfs_log_fd, msg, strlen(msg));
    fsync(sdfs_log_fd);
}