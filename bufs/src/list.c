#include "dir.h"
#include "super.h"
#include "inode.h"
#include "user.h"

#include <unistd.h>
#include <fcntl.h>
#include <malloc.h>


int bufs_list(bufs_fs_t* fs_info, int dinum, bufs_stat_t** buffer)
{
    size_t array_size;
    int fd; 
    bufs_super_block_t sb;
    bufs_inode_t dinode;
    bufs_dirent_t* array_buf;

    fd = fs_info->fd;
    bufs_super_get(fd, &sb); 

    if(!bufs_inode_exist(fs_info->fd, sb.inode_bitmap_address, dinum))
        return -2;

    bufs_inode_get(fd, sb.inode_region_address, dinum, &dinode);
    if(dinode.type != BUFS_DIRECOTRY)
        return -1;
        
    array_size = bufs_dir_ent_array(fd, dinum, &array_buf);
    *buffer = (bufs_stat_t*)malloc(sizeof(bufs_stat_t) * array_size);
    for(size_t i = 0; i < array_size; ++i)
    {
        bufs_inode_t cur_inode;
        bufs_dirent_t ent;

        ent = array_buf[i];
        bufs_inode_get(fd, sb.inode_region_address, ent.inum, &cur_inode);
        (*buffer)[i].inum = ent.inum;
        (*buffer)[i].type = cur_inode.type;
        (*buffer)[i].size = cur_inode.size;
    }
    bufs_dir_ent_freearray(&array_buf);
 
    return array_size; 
}

void bufs_freelist(bufs_stat_t** buffer)
{
    free(*buffer);
}