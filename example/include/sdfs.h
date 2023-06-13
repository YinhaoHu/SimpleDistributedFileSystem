// Application programming interface of Simple Distributed File System. 

#ifndef _SDFS_CLIENT_CORE_H_
#define _SDFS_CLIENT_CORE_H_

#define SDFS_OK (0)
#define SDFS_BAD (-1)

#define SDFS_ROOT_INUM (0)

#define SDFS_DIRECOTRY (0)
#define SDFS_REGULAR_FILE (1)

#define SDFS_RC_INITIALIZER {(char*)0, 0,0}

#define SDFS_RC_STATUS_OK               (0x00000000)
#define SDFS_RC_STATUS_BAD              (0xffffffff)

#define SDFS_INIT_RC_SOCKET             (0x8f001001)
#define SDFS_INIT_RC_BUF                (0x8f001002)

#ifdef _TEST_
#define SDFS_BUFFER_DIR_NAME             "test/buffer"
#else
#define SDFS_BUFFER_DIR_NAME             "buffer"
#endif

typedef unsigned int sdfs_size_t;
typedef int sdfs_int_t;
typedef unsigned int sdfs_uint_t;
typedef int sdfs_sock_fd_t;
 
typedef struct _sdfs_return_code_{
    char* text;
    sdfs_uint_t status;
    sdfs_uint_t code;
}sdfs_return_code_t;

typedef struct _sdfs_stat{
    sdfs_uint_t inum;
    sdfs_uint_t type;
    sdfs_size_t size;
}sdfs_stat_t;


// Call this function before any other functions in the SDFS.
// Check the return_code to see what was going on.
sdfs_return_code_t sdfs_init(char *hostname, char* service);

// return SDFS_LOOKUP_NOT_FOUND if not found,
// SDFS_BAD on error(Server crash, etc.)
// inode number if found.SS
sdfs_int_t sdfs_lookup(sdfs_uint_t pinum, char* name);

// Return SDFS_OK on success, SDFS_SERVER_CRASH on server
// crash, SDFS_STAT_INUM if inum does not exist.
sdfs_int_t sdfs_stat(sdfs_uint_t inum, sdfs_stat_t* buf);

// Return the nbytes of written successfully.
// Negative return value if error happens, to see what was
// going on, see the macros.
sdfs_int_t sdfs_write(sdfs_uint_t inum, char *buf, sdfs_size_t offset, sdfs_size_t nbytes);

// Return the nbytes of read successfully.
// Negative return value if error happens, to see what was
// going on, see the macros.
sdfs_int_t sdfs_read(sdfs_uint_t inum, char *buf, sdfs_size_t offset, sdfs_size_t nbytes);

// Return inode number created on success, SDFS_BAD on name is NULL.
// Other errors can be detected by the associated macros.
sdfs_int_t sdfs_create(sdfs_uint_t pinum, char *name, sdfs_uint_t type);

// Return SDFS_OK on success, SDFS_BAD on name is NULL.
// Other errors can be detected by the associated macros.
sdfs_int_t sdfs_unlink(sdfs_uint_t dinum, char *name);

// Return the size of the list, -1 if dinum is invalid.
// User takes the responsibility to call freelist()
sdfs_int_t sdfs_list(sdfs_uint_t dinum, sdfs_stat_t **buffer); 
void sdfs_freelist(sdfs_stat_t **buffer);


#endif