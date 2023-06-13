#include "sdfs_core.h"
#include "sdfs_config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>


extern int sdfs_file_timestamps_fd;
extern long sdfs_file_timestamps[SDFS_MAX_INODES];

sdfs_return_code_t sdfs_file_timestamps_init()
{ 
    sdfs_return_code_t rc = SDFS_RETURN_CODE_INITIALIZER;
    ssize_t writen_bytes;
    struct stat sbuf;
    size_t stamps_size = (SDFS_MAX_INODES * sizeof(long));

    memset(&sbuf, 0, sizeof(struct stat));    
    sdfs_file_timestamps_fd = open(SDFS_SERVER_FILE_TIMESTAMPS_FILE_NAME, O_CREAT | O_RDWR, 0666);

    if( sdfs_file_timestamps_fd == -1)
    {
        rc.status = SDFS_RETURN_CODE_STATUS_FAILURE;
        rc.text = "Fail in creating or open file time stamps file.\n";
        return rc;
    }

    stat(SDFS_SERVER_FILE_TIMESTAMPS_FILE_NAME, &sbuf);
    if(sbuf.st_size != stamps_size)
    {
        memset(sdfs_file_timestamps, 0, stamps_size);
        writen_bytes = write(sdfs_file_timestamps_fd, sdfs_file_timestamps, stamps_size);
        fsync(sdfs_file_timestamps_fd);
        if(writen_bytes != stamps_size)
        {
            rc.status = SDFS_RETURN_CODE_STATUS_FAILURE;
            rc.text = "Fail in fixing time stamps file.\n";
        }
    }
    return rc;
}


void sdfs_file_timestamps_update(sdfs_int_t inum, sdfs_update_file_timestamps_e mode)
{
    switch (mode)
    {
    case SDFS_UPDATE_FILE_TIMESTAMPS_ZERO:
        sdfs_file_timestamps[inum] = 0;
        break;
    case SDFS_UPDATE_FILE_TIMESTAMPS_CURRENT:
        sdfs_file_timestamps[inum] = time(NULL);
        break;
    } 
    lseek(sdfs_file_timestamps_fd, inum * sizeof(long), SEEK_SET);
    write(sdfs_file_timestamps_fd, sdfs_file_timestamps + inum , 
        sizeof(long));
    fsync(sdfs_file_timestamps_fd);
}

long sdfs_file_timestamps_get(sdfs_uint_t inum)
{
    return sdfs_file_timestamps[inum];
}


void sdfs_file_timestamps_load()
{
    ssize_t readn_bytes, expected_readn_bytes;
    
    sdfs_file_timestamps_init();

    expected_readn_bytes = sizeof(sdfs_file_timestamps);
    lseek(sdfs_file_timestamps_fd, 0, SEEK_SET);
    readn_bytes = read(sdfs_file_timestamps_fd, sdfs_file_timestamps, 
            expected_readn_bytes);
    if(readn_bytes != expected_readn_bytes)
    {
        fprintf(stderr, "Load file stamps error! [readn = %ld]\n", readn_bytes);
        exit(EXIT_FAILURE);
    }
}