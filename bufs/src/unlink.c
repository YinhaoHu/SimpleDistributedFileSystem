#include"dir.h"
#include"inode.h"
#include"super.h"
#include"datablock.h"
#include"user.h"

#include <stdio.h>
#include<unistd.h>
#include<string.h>

int bufs_unlink(bufs_fs_t* fs_info, int pinum, char *name)
{
    int fd, inum, rc;
    bufs_super_block_t sb;
    bufs_inode_t pinode, inode;

    fd = fs_info->fd;
    rc = 0;
    bufs_super_get(fd, &sb);

    if(bufs_inode_exist(fs_info->fd, sb.inode_bitmap_address, pinum) == -1)
        return -1;

    bufs_inode_get(fd, sb.inode_region_address, pinum, &pinode);
    if(pinode.type != BUFS_DIRECOTRY)
        return -2;

    if(strlen(name) >= BUFS_DIRECOTRY_ENTRY_NAME_MAXLEN) 
        return -3;
        
    inum = bufs_dir_ent_inum(fd, pinum, name);
    if( inum == -1)
        return -4;
        
    bufs_inode_get(fd, sb.inode_region_address, inum, &inode);
    switch (inode.type)
    {
    case BUFS_REGULAR_FILE:
        rc = bufs_dir_ent_del(fd, pinum, name);
        for(int i = 0; inode.direct[i] != 0; ++i) 
            bufs_db_free(fd, sb.data_bitmap_address, sb.data_region_address,inode.direct[i]);
        bufs_inode_free(fd, sb.inode_bitmap_address, inum); 
        break;
    case BUFS_DIRECOTRY:
        bufs_dirent_t* ent_array;
        int size;

        size = bufs_dir_ent_array(fs_info->fd, inum, &ent_array);
         
        if(size < 0) 
            rc = -5;  
        else if( size != 2) 
            rc = -6; 
        else
        {
            rc = bufs_dir_ent_del(fd, pinum, name);
            for(int i = 0; inode.direct[i] != 0; ++i) 
                bufs_db_free(fd, sb.data_bitmap_address, sb.data_region_address,inode.direct[i]);
            bufs_inode_free(fd, sb.inode_bitmap_address, inum); 
        }
        break;
    default:
        break;
    }

    return rc;
}
