#include "snftp/snftp.h"
#include "sdfs_inet.h"
#include "sdfs.h"
#define _GNU_SOURCE
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>

// Return SDFS_OK on buffer direcotry existing,
// SDFS_BAD on not.
static sdfs_int_t sdfs_check_buffer_dir();
// Return SDFS_OK on success, SDFS_BAD on failure.
static sdfs_int_t sdfs_make_buffer_dir();


sdfs_return_code_t sdfs_init(char* hostname, char* service)
{ 
    sdfs_return_code_t rc = SDFS_RC_INITIALIZER;

    rc.status = SDFS_RC_STATUS_OK;

    if(sdfs_connect(hostname, service) < 0)
    {
        rc.code = SDFS_INIT_RC_SOCKET;
        rc.status = SDFS_RC_STATUS_BAD;
        rc.text = "Bad connection in sdfs_init.\n";
        return rc;
    }
    if(sdfs_check_buffer_dir() != SDFS_OK)
    {
        if(sdfs_make_buffer_dir() != SDFS_OK)
        {
            rc.code = SDFS_INIT_RC_BUF;
            rc.status = SDFS_RC_STATUS_BAD;
            rc.text = "Bad generating buffer direcotry.\n"; 
            return rc;
        }
    }

    return rc;
}

static sdfs_int_t sdfs_check_buffer_dir()
{
    DIR* buf_dir = opendir(SDFS_BUFFER_DIR_NAME);

    if(buf_dir == NULL)
        return SDFS_BAD;
    closedir(buf_dir);
    return SDFS_OK;
}


static sdfs_int_t sdfs_make_buffer_dir()
{
    if(mkdir(SDFS_BUFFER_DIR_NAME, 0777) != 0)
    {
        perror(strerror(errno));
        return SDFS_BAD;
    }
    else
        return SDFS_OK;
}