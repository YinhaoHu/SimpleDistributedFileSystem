#include "user.h"
#include <unistd.h>
#include <fcntl.h>


int bufs_fs_mount(char* img_name, bufs_fs_t* fs_info)
{
    int fd = open(img_name, O_RDWR);

    if(fd == -1)
        return -1;
    else 
    {
        fs_info->img_name = img_name;
        fs_info->fd = fd;
        return 0;
    }
}


int bufs_fs_unmount(bufs_fs_t* fs_info)
{
    if(fs_info->fd == -1)
        return -1;
    else 
    {
        close(fs_info->fd);
        fs_info->fd = -1;
        return 0;
    }
}