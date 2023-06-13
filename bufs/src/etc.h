#ifndef _BUFS_ETC_H_
#define _BUFS_ETC_H_

#include"types.h"
#include<unistd.h>
/*
    The functions below are wrapper or some higher abstraction
    functions for convenience and avoiding error.

*/ 


// Use carefully, DO NOT use ++, --.
#define MAX(x, y) ( ((x) > (y)) ? (x) : (y)) 
#define MIN(x, y) ( ((x) < (y)) ? (x) : (y))


int byte_set(char* byte, int bit_idx, int val);
int byte_index(char byte, int val);
int byte_lookup(char* byte, int bit_idx);

__off_t seek_block(int fd, bufs_addr_t block_address);

int bitmap_alloc(int fd, bufs_addr_t bitmap_address, int bits_num);
int bitmap_free(int fd, bufs_addr_t bitmap_address, int bit_num);
int bitmap_index_value(int fd, bufs_addr_t bitmap_address, int bit_idx);

#endif