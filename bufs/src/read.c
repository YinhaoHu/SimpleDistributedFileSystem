#include "inode.h"
#include "datablock.h"
#include "super.h"
#include "etc.h"
#include "user.h"
 
#include <string.h>
#include <unistd.h> 
#include <malloc.h>


long bufs_read(bufs_fs_t* fs_info, int inum, char* buf, int offset, long nbytes)
{
    int fd, boundry, buf_idx;
    long read_bytes;
    bufs_super_block_t sb;
    bufs_inode_t inode;

    fd = fs_info->fd;
    bufs_super_get(fd, &sb);
    if(!bufs_inode_exist(fs_info->fd, sb.inode_bitmap_address, inum))
        return -3;
    bufs_inode_get(fd, sb.inode_region_address, inum, &inode);
 
    if(inode.type != BUFS_REGULAR_FILE)
        return -1;
    if(inode.size < offset)
        return -2; 
    boundry = MIN(BUFS_DIRECT_PTRS*BUFS_BLOCK_SIZE, offset+(int)nbytes);
    buf_idx = 0;
    read_bytes = 0; 
    for(int bid = offset / BUFS_BLOCK_SIZE; offset < boundry && nbytes > 0; ++bid)
    {
        bufs_addr_t db_addr;
        size_t to_read_nbytes;
        int db_idx_begin; 
        db_idx_begin = offset % BUFS_BLOCK_SIZE;
        to_read_nbytes = MIN(BUFS_BLOCK_SIZE-db_idx_begin, (int)nbytes);
        if(inode.size < offset+to_read_nbytes) 
            break; 
        db_addr = inode.direct[bid];
        lseek(fd, db_addr*BUFS_BLOCK_SIZE + db_idx_begin, SEEK_SET);
        read_bytes += read(fd, buf+buf_idx, to_read_nbytes);
        buf_idx += to_read_nbytes;
        offset += to_read_nbytes;
        nbytes -= to_read_nbytes; 
    }       
    return read_bytes; 
}