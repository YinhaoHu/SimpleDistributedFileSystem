#include"inode.h"
#include"param.h"
#include"etc.h"

#include<unistd.h>
#include<string.h>
// Core Code: begin

//return allocated inum, -1 on failure.
int bufs_inode_alloc(int fd, int bitmap_address, int num_inodes)
{
    return bitmap_alloc(fd, bitmap_address, num_inodes) ;
}

//return 0 on success, -1 on failure
int bufs_inode_free(int fd, int bitmap_address, int inum)
{
    return bitmap_free(fd, bitmap_address, inum);
}

//return 0 on not exist, otherwise 1.
int bufs_inode_exist(int fd, bufs_addr_t bitmap_address, int inum)
{
    return bitmap_index_value(fd, bitmap_address, inum);
}
 
int bufs_inode_set(int fd, int inode_region_address, int inum, const bufs_inode_t* inode)
{
    int written;

    lseek(fd, BUFS_BLOCK_SIZE * inode_region_address + inum * sizeof(bufs_inode_t), SEEK_SET);
    written = write(fd, inode, sizeof(bufs_inode_t));
    return written == sizeof(bufs_inode_t) ? 0 : -1;
}

 //return 0 on success, -1 on read system failure.
int bufs_inode_get(int fd, int inode_region_address, int inum, bufs_inode_t* buffer)
{
    int readbytes;

    lseek(fd, BUFS_BLOCK_SIZE * inode_region_address + inum * sizeof(bufs_inode_t), SEEK_SET);
    readbytes = read(fd, buffer, sizeof(bufs_inode_t));
    return readbytes == sizeof(bufs_inode_t) ? 0 : -1;
}

// init the inode of the direcotry in the memory.
int bufs_inode_init_dinode(int fd, bufs_addr_t inode_region_address, int inum, bufs_addr_t db_addr)
{
    bufs_inode_t inode;

    memset(inode.direct, 0, sizeof(unsigned int) * BUFS_DIRECT_PTRS);
    inode.direct[0] = db_addr;
    inode.type = BUFS_DIRECOTRY;
    inode.size = 2 * sizeof(bufs_dirent_t);

    return bufs_inode_set(fd, inode_region_address, inum, &inode);
}

// init the variable inode
int bufs_inode_init(bufs_inode_t* inode, int type)
{
    if(inode == NULL)
        return -1;

    memset(inode, 0, sizeof(bufs_inode_t));
    inode->type = type;
    return 0;
}

// Core Code: end