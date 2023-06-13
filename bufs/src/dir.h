#ifndef _BUFS_DIR_H_
#define _BUFS_DIR_H_

#include"types.h"

//return 0 on success, -1 on failure.
int bufs_dir_init(int fd, int dir_db_address, int pinum, int inum);

//return 0 on success, -1 on failure.
int bufs_dir_ent_add(int fd, int dinum, bufs_dirent_t* entry);

//return 0 on success, -1 on failure.
int bufs_dir_ent_init(bufs_dirent_t* entry, int inum, char* name);

//return 0 on success, -1 on failure.
int bufs_dir_ent_del(int fd, int dinum, char* name);

//return inum of the name in the direcotry on success, -1 on not found.
int bufs_dir_ent_inum(int fd, int dinum, char* name);

//return total size of array on success, -1 on failure.
int bufs_dir_ent_array(int fd, int dinum, bufs_dirent_t** array_ptr);

void bufs_dir_ent_freearray(bufs_dirent_t** array_ptr);

#endif 