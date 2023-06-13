#include "user.h"
#include "../src/dir.h"
#include "../src/super.h"
#include "../src/inode.h"

#include <unistd.h>
#include <fcntl.h>
#include <string.h>


int bufs_lookup(bufs_fs_t* fs_info, int pinum, char* name, int* inum_buf)
{
    size_t array_size;
    int fd, rc; 
    bufs_super_block_t sb;
    bufs_inode_t pinode;
    bufs_dirent_t* array_buf;

    fd = fs_info->fd;
    bufs_super_get(fd, &sb); 
    bufs_inode_get(fd, sb.inode_region_address, pinum, &pinode);

    if(!bufs_inode_exist(fd, sb.inode_bitmap_address, pinum))
        return -2;

    if(pinode.type != BUFS_DIRECOTRY)
        return -1;
    array_size = bufs_dir_ent_array(fd, pinum, &array_buf);

    rc = 0;
    for(int i = 0; i < array_size; ++i)
    {
        if(strcmp(array_buf[i].name, name) == 0)
        {
            if(inum_buf != NULL)
                *inum_buf = array_buf[i].inum;
            rc = 1;
            break;
        }
    }

    bufs_dir_ent_freearray(&array_buf); 
    return rc;  
}