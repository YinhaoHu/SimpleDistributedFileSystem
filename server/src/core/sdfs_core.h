#ifndef _SDFS_CORE_H_
#define _SDFS_CORE_H_

#define SDFS_MAX(x,y) ( ((x) > (y)) ? (x) : (y) )
#define SDFS_MIN(x,y) ( ((x) < (y)) ? (x) : (y) )


// Extract from mkfs.h in the BUFS mannually.
// This is not good actually.
#define SDFS_MAX_INODES (32)

#define SDFS_OK (0)
#define SDFS_BAD (-1)

#define SDFS_RETURN_CODE_STATUS_SUCCESS (0)
#define SDFS_RETURN_CODE_STATUS_FAILURE (-1)

#define SDFS_RETURN_CODE_INITIALIZER {(char*)0, SDFS_RETURN_CODE_STATUS_SUCCESS, 0}
#define SDFS_LOG_MSG_LEN (64)

typedef unsigned int sdfs_uint_t;
typedef int sdfs_int_t;
typedef int sdfs_fd_t;
typedef unsigned int sdfs_size_t;
typedef int sdfs_sock_fd_t;

typedef struct _sdfs_return_code_t{
    char* text;
    sdfs_int_t status;
    sdfs_int_t code;
} sdfs_return_code_t;

typedef enum{
    SDFS_UPDATE_FILE_TIMESTAMPS_CURRENT = 0,
    SDFS_UPDATE_FILE_TIMESTAMPS_ZERO
}sdfs_update_file_timestamps_e;

void sdfs_file_timestamps_update(sdfs_int_t inum, sdfs_update_file_timestamps_e mode);
long sdfs_file_timestamps_get(sdfs_uint_t inum);
sdfs_return_code_t sdfs_file_timestamps_init();
void sdfs_file_timestamps_load();

sdfs_return_code_t sdfs_log_init();
void sdfs_log_write_rc(sdfs_return_code_t* rc);

// Parameter msg should have '\n' as end valid character.
void sdfs_log_write_msg(char* msg);

#endif