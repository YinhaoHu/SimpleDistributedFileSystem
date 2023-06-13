#ifndef _BUFS_INODE_H_
#define _BUFS_INODE_H_

#include"types.h"
 
int bufs_inode_alloc(int fd, int bitmap_address, int num_inodes ); 
int bufs_inode_free(int fd, int bitmap_address, int inum); 
int bufs_inode_exist(int fd, bufs_addr_t bitmap_arress, int inum);
int bufs_inode_set(int fd, int inode_region_address, int inum, const bufs_inode_t* inode); 
int bufs_inode_get(int fd, int inode_region_address, int inum, bufs_inode_t* buffer);
int bufs_inode_init_dinode(int fd, bufs_addr_t inode_region_address, int inum, bufs_addr_t db_addr);
int bufs_inode_init(bufs_inode_t* inode, int type);

#endif