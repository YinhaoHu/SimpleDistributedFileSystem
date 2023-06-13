#include "inode.h"
#include "datablock.h"
#include "super.h"
#include "param.h"
#include "etc.h"
#include "mkfs.h"
#include "user.h"

#include <unistd.h>  
#include <stdio.h>

/*
    Write Algorithm:

    Basic:
        offset means the beginning index in the whole data area of the inum.
        nbytes means the remained bytes need writting.

    Loop: block by block
        get current block should be written.
            if size ok: next step.
            else: alloc one block.
        get current block idx written begins with.(check size)
        seek to the target position.
        write block
        update(offset, nbytes)
    
    Border cases:
        1 boundary size will not be over 30 blocks for file and 4 blocks for dir.
        2 offset might be 2 blocks more than the original size.(limit this)
*/


long bufs_write(bufs_fs_t* fs_info, int inum, char* buf, int offset, long nbytes)
{ 
    /*
        Here is a little buggy. offset should be long or unsigned int.
        But I forced the type conversion below.
        In this small file size file system, it does not matter.
    */
    int fd, boundry, buf_idx;
    long written_bytes;
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
    boundry = MIN(BUFS_DIRECT_PTRS*BUFS_BLOCK_SIZE, offset+(int)nbytes) ;
    buf_idx = 0;
    written_bytes = 0;
    for(int bid = offset / BUFS_BLOCK_SIZE; offset < boundry && nbytes > 0; ++bid)
    { 
        bufs_addr_t db_addr;
        size_t to_write_nbyets;
        int db_idx_begin;
 
        db_idx_begin = offset % BUFS_BLOCK_SIZE;
        to_write_nbyets = MIN((int)BUFS_BLOCK_SIZE - db_idx_begin, (int)nbytes); 
        if(inode.size < offset+to_write_nbyets)
        {    
            inode.direct[bid] = bufs_db_alloc(fd, sb.data_bitmap_address, sb.num_data, 
              sb.data_region_address);
            inode.size = offset+to_write_nbyets; 
        }
        db_addr = inode.direct[bid];
        lseek(fd, db_addr*BUFS_BLOCK_SIZE + db_idx_begin, SEEK_SET);
        written_bytes += write(fd, buf+buf_idx, to_write_nbyets);
        buf_idx += to_write_nbyets;
        offset += to_write_nbyets;
        nbytes -= to_write_nbyets; 

    }     
    bufs_inode_set(fd, sb.inode_region_address, inum, &inode); 
    fsync(fd);  

    return written_bytes; 
}

