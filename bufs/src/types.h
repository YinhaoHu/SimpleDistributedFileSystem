#ifndef _BUFS_TYPES_H_
#define _BUFS_TYPES_H_

#include "param.h"

typedef unsigned int bufs_addr_t;
typedef long bufs_size_t;

typedef struct{
    int type;                               // direcotry OR regular file
    int size;                               // in bytes
    unsigned int direct[BUFS_DIRECT_PTRS];  // direct pointers to the data blocks
}bufs_inode_t;

typedef struct 
{ 
    int inum;                               // inode number or -1 means unused 
    char name[BUFS_DIRECOTRY_NAME_MAXLEN];  // direcotry name
}bufs_dirent_t;

typedef struct _bufs_super_block
{
    int inode_bitmap_address;   // block address
    int inode_bitmap_len;       // in block
    int data_bitmap_address;    // block address
    int data_bitmap_len;        // in block
    int inode_region_address;   // block address
    int inode_region_len;       // in block
    int data_region_address;    // block address
    int data_region_len;        // in block
    int num_inodes;             // the number of the inodes 
    int num_data;               // the number of the data blocks
}bufs_super_block_t;


#endif