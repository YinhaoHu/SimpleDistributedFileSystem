#include "inode.h"
#include "super.h"
#include "user.h"

#include <unistd.h>


int bufs_stat(bufs_fs_t* fs_info, int inum, bufs_stat_t* buffer)
{
    int fd, rc;
    bufs_inode_t inode;
    bufs_super_block_t sb;

    if(buffer == NULL)
        return -1;

    fd = fs_info->fd;
    bufs_super_get(fd, &sb);

    if(bufs_inode_exist(fs_info->fd, sb.inode_bitmap_address, inum) == 0)
        return -1;

    rc = bufs_inode_get(fd, sb.inode_region_address, inum, &inode);
    if(rc == -1)
        return -2;

    buffer->inum = inum;
    buffer->type = inode.type;
    buffer->size = inode.size;
 
    return 0; 
}
