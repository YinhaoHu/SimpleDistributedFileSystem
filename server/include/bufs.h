#ifndef _BUFS_USER_H_
#define _BUFS_USER_H_

#ifdef _BUFS_MKFS_INFO
#include "mkfs.h"
#endif

//Include '\0'
#define BUFS_DIRECOTRY_ENTRY_NAME_MAXLEN (28)

typedef struct{
    int inum;
    int type;
    int size;
} bufs_stat_t;

typedef struct{
    char* img_name;
    int fd;
} bufs_fs_t;

// Open the image file which was already made by mkfs.
// Return 0 on success, -1 on failure.
int bufs_fs_mount(char* img_name, bufs_fs_t* fs_info);

// Close the image file.
// Return 0 on success, -1 on failure.
int bufs_fs_unmount(bufs_fs_t* fs_info);


// Lookup whether the name in the pinum exist. If exist and inum_buf is 
// not NULL, inum_buf will store the inode number. Return 1 if found,
// 0 if not found, -1 if pinum is not directory, -2 pinum does not exist.
int bufs_lookup(bufs_fs_t* fs_info, int pinum, char* name, int* inum_buf);


// Return the nbytes of write, -1 on type error, -2 on offset illegal.
// -3 on inum does not exist.
// It is possible the written bytes are less than nbytes because of
// offset+nbytes is greater than the limitation of BUFS.
long bufs_write(bufs_fs_t* fs_info, int inum, char* buf, int offset, long nbytes);


// Return the nbytes of read, -1 on type error, -2 on offset illegal.
// -3 on inum does not exist.
// It is possible the read bytes are less than nbytes because of
// offset+nbytes is  greater than the limitation of BUFS.
long bufs_read(bufs_fs_t* fs_info, int inum, char* buf, int offset, long nbytes);

// Return 0 on success, -1 on not found, -2 on system error.
int bufs_stat(bufs_fs_t* fs_info, int inum, bufs_stat_t* buffer);


// Return inum on success, -1 on no space, -2 on non-dir pinum.
// -3 on non-exist pinum, -4 on too long name, -5 on wrong type. 
// -6 on name already existing. Believe the user to give a valid name pointer.
int bufs_create(bufs_fs_t* fs_info, int pinum, char *name, int type);

// Return 0 on success, -1 on pinum does not exist,
// -2 on pinum is not of direcotry, -3 on too long name.
// -4 on no such name in the direcotry, -5 on system error.
// -6 on non-empty direcotry name.
int bufs_unlink(bufs_fs_t* fs_info, int pinum, char *name);

// Return array size on success, -1 on dinum is not direcotry inode number. 
// -2 if dinum is not exist. If error, no new allocated memory. Don't free.
// Put the array in the buffer. Buffer needs to be free manually
int bufs_list(bufs_fs_t* fs_info, int dinum, bufs_stat_t** buffer);

// Free the list from bufs_list().
void bufs_freelist(bufs_stat_t** buffer);

#endif