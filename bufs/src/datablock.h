#ifndef _BUFS_BLOCK_H_
#define _BUFS_BLOCK_H_

#include"types.h"
#include"param.h"
#include <unistd.h>

bufs_addr_t bufs_db_alloc(int fd, bufs_addr_t bitmap_address, int num_data, bufs_addr_t db_region_addr);
int bufs_db_free(int fd, bufs_addr_t bitmap_address,
    bufs_addr_t db_region_address ,bufs_addr_t db_address);

int bufs_db_read(int fd, unsigned int  db_address, char* buf);
int bufs_db_write(int fd, unsigned int  db_address, char*db);

#endif