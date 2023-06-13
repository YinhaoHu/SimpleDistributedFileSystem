#include"mkfs.h"
#include"super.h"
#include"dir.h"
#include"datablock.h"
#include"inode.h"

#include<sys/stat.h>
#include<unistd.h>
#include<fcntl.h>
#include<malloc.h>
#include<string.h>
#include<assert.h>

static int make_empty(int fd);
static int set_super_block(int fd);
static int init_root(int fd);

//return 0 on success, -1 on failure.
int mkfs(char* fs_img)
{
    int fd , rc;
    fd = open(fs_img, O_RDWR | O_TRUNC | O_CREAT, S_IRUSR | S_IWUSR);

    rc = make_empty(fd);
    assert(rc == 0);

    rc = set_super_block(fd);
    assert(rc == 0);

    rc = init_root(fd);
    assert(rc == 0);
 
    close(fd);
    
    return rc;
}

static int make_empty(int fd)
{
    size_t size, rc;
    char* total_bytes_buf;

    size = BUFS_BLOCK_SIZE * BUFS_FS_BLOCKS;
    total_bytes_buf = (char*)malloc(size);
    memset(total_bytes_buf, 0, size);  
    lseek(fd, 0, SEEK_SET);
    rc = write(fd, total_bytes_buf, size);
    free(total_bytes_buf);

    return rc == size ? 0 : -1;
}

static int set_super_block(int fd)
{
    int rc;
    bufs_super_block_t sb;
    size_t size;

    size = sizeof(sb);

    sb.data_bitmap_address = BUFS_DATA_BITMAP_ADDRESS;
    sb.data_bitmap_len = BUFS_DATA_BITMAP_LEN;
    sb.data_region_address = BUFS_DATA_REGION_ADDRESS;
    sb.data_region_len = BUFS_DATA_REGION_LEN;

    sb.inode_bitmap_address = BUFS_INODE_BITMAP_ADDRESS;
    sb.inode_bitmap_len = BUFS_INODE_BITMAP_LEN;
    sb.inode_region_address = BUFS_INODE_REGION_ADDRESS;
    sb.inode_region_len = BUFS_INODE_REGION_LEN;

    sb.num_data = BUFS_NUM_DATA;
    sb.num_inodes = BUFS_NUM_INODES;

    lseek(fd, 0, SEEK_SET);
    rc = write(fd, &sb, size);
    return rc == size ? 0 : -1;
}

static int init_root(int fd)
{ 
    int inum; 
    bufs_addr_t db_addr;

    db_addr = bufs_db_alloc(fd, BUFS_DATA_BITMAP_ADDRESS, BUFS_NUM_DATA, BUFS_DATA_REGION_ADDRESS);
    inum = bufs_inode_alloc(fd, BUFS_INODE_BITMAP_ADDRESS, BUFS_NUM_INODES);
    
    bufs_inode_init_dinode(fd, BUFS_INODE_REGION_ADDRESS, inum, db_addr);
    bufs_dir_init(fd, db_addr, inum, inum);
 
    return ((inum == BUFS_SUPER_INUM) || (db_addr == BUFS_DATA_REGION_ADDRESS)) ? 0 : -1; 
}