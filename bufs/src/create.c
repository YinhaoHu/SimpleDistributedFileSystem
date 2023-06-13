/*
    Implementation for bufs_create()

    Algorithm:
        1 alloc one inode
        2 init the new inode
        3 add entry in the parent
*/
#include"param.h"
#include"types.h"
#include"inode.h"
#include"dir.h"
#include"datablock.h"
#include"super.h"

#include"user.h"

#include<assert.h>
#include<string.h>
#include<unistd.h>
#include<fcntl.h>
#include<stdio.h>


int bufs_create(bufs_fs_t* fs_info, int pinum, char *name, int type)
{ 
    int fd, inum;
    bufs_super_block_t sb;
    bufs_inode_t inode, pinode;
    bufs_dirent_t ent; 


    if(strlen(name) >= BUFS_DIRECOTRY_ENTRY_NAME_MAXLEN)
        return -4;

    if(!BUFS_ISVALID_INODE_TYPE(type))
        return -5;

    fd = fs_info->fd,
    bufs_super_get(fd, &sb);
    bufs_inode_get(fs_info->fd, sb.inode_region_address, pinum, &pinode);
    if(pinode.type != BUFS_DIRECOTRY)
        return -2;
    if(!bufs_inode_exist(fs_info->fd, sb.inode_bitmap_address, pinum))
        return -3;

    if(bufs_dir_ent_inum(fs_info->fd, pinum, name) != -1)
        return -6;

    inum = bufs_inode_alloc(fd, sb.inode_bitmap_address, sb.num_inodes);
    bufs_inode_init(&inode, type);

    ent.inum = inum;
    strcpy(ent.name, name);  
    bufs_dir_ent_add(fd, pinum, &ent);

    if(type == BUFS_DIRECOTRY)
    {
        inode.direct[0] = bufs_db_alloc(fs_info->fd, sb.data_bitmap_address, 
            sb.num_data, sb.data_region_address);
        bufs_dir_init(fs_info->fd, inode.direct[0], pinum, inum);
    }

    bufs_inode_set(fd, sb.inode_region_address, inum, &inode);

    
 
    return inum; 
}