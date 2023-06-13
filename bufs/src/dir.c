#include"dir.h"
#include"fcntl.h"
#include"types.h"
#include"inode.h"
#include"super.h"

#include<string.h>
#include<unistd.h>
#include<malloc.h>

// Return 0 on success, -1 on empty, -2 on error.
static int bufs_dirent_readone(int fd, bufs_dirent_t* buf);

// Return the index of the target, put the inode into the buf(If buf is not NULL),
// -1 if not found.
static int bufs_dirent_find(int fd, unsigned int dir_db_addr,char* name, bufs_dirent_t* buf);
static unsigned int bufs_dirent_dbaddr(int fd, int dinum);
static int bufs_dirent_is_empty(bufs_dirent_t* ent);

int bufs_dir_init(int fd, int dir_db_address, int pinum, int inum)
{
    int rc;
    bufs_dirent_t ents[2] = { {inum, "."}, {pinum, ".."}}; 
    
    lseek(fd, dir_db_address * BUFS_BLOCK_SIZE, SEEK_SET);
    rc = write(fd, (const void*)ents, sizeof(ents)); 
    return (rc == sizeof(ents)) ? 0 : -1;
}

int bufs_dir_ent_add(int fd, int dinum, bufs_dirent_t* entry)
{
    int rc, idx;
    bufs_super_block_t super;
    bufs_inode_t inode;
    bufs_dirent_t temp;

    bufs_super_get(fd, &super);
    rc = bufs_inode_get(fd, super.inode_region_address, dinum, &inode);
    if(inode.type != BUFS_DIRECOTRY || rc < 0)
        return -1;

    lseek(fd, inode.direct[0] * BUFS_BLOCK_SIZE, SEEK_SET); 
    idx=0;
    while(bufs_dirent_readone(fd,  &temp) == 0 && idx < BUFS_DIRECOTRY_MAX_ENTRIES)
        idx++;

    if(idx >= BUFS_DIRECOTRY_MAX_ENTRIES)
        return -1;
    lseek(fd, lseek(fd, 0,SEEK_CUR)- sizeof(bufs_dirent_t), SEEK_SET);
    rc = write(fd, (const void*)entry, sizeof(bufs_dirent_t));

    return rc == sizeof(bufs_dirent_t) ? 0 : -1;
}

//return 0 on sucess, -1 on notFound
int bufs_dir_ent_del(int fd, int dinum, char* name)
{
    // 
    int ent_idx, dir_db_address, old_size, new_size;
    bufs_dirent_t *array;

    old_size = bufs_dir_ent_array(fd, dinum, &array);    
    new_size = old_size - 1;
    dir_db_address = bufs_dirent_dbaddr(fd, dinum);
    ent_idx = bufs_dirent_find(fd, dir_db_address,name, NULL);

    for(int i = ent_idx; i < new_size; ++i)
        memcpy(&(array[i]), &(array[i+1]), sizeof(bufs_dirent_t));

    memset(&(array[new_size]), 0, sizeof(bufs_dirent_t));

    lseek(fd, BUFS_BLOCK_SIZE * dir_db_address, SEEK_SET);
    if(write(fd, array, old_size * sizeof(bufs_dirent_t)) < 0)
        return -1;
    bufs_dir_ent_freearray(&array);

    return ent_idx == -1 ? -1 : 0;
}

int bufs_dir_ent_inum(int fd, int dinum, char* name)
{
    int rc;
    unsigned int dbaddr;
    bufs_dirent_t ent;

    dbaddr = bufs_dirent_dbaddr(fd, dinum);  
    rc = bufs_dirent_find(fd, dbaddr,name ,&ent); 

    return (rc == -1) ? -1 : ent.inum;
}

int bufs_dir_ent_array(int fd, int dinum, bufs_dirent_t** array_ptr)
{
    int rc = 0;
    bufs_super_block_t super;
    bufs_inode_t inodebuf;

    rc = bufs_super_get(fd, &super);
    if(rc < 0)
        return -1;

    bufs_inode_get(fd, super.inode_region_address, dinum, &inodebuf);
    if(inodebuf.type != BUFS_DIRECOTRY)
        return -1;

    int dir_db_address = inodebuf.direct[0];
    int buf_size = BUFS_DIRECOTRY_MAX_ENTRIES * (sizeof(bufs_dirent_t));
    int ent_size = sizeof(bufs_dirent_t), ent_idx = 0;
    
    bufs_dirent_t temp;

    *array_ptr = (bufs_dirent_t*)malloc(buf_size);
    memset(*array_ptr, 0, buf_size);

    lseek(fd, dir_db_address * BUFS_BLOCK_SIZE, SEEK_SET); 
    while(bufs_dirent_readone(fd, &temp) == 0 )
    {
        memcpy(&((*array_ptr)[ent_idx]), &temp, ent_size);
        ent_idx++; 
    }

    return rc < 0 ? rc : (ent_idx );
}

int bufs_dir_ent_init(bufs_dirent_t* entry, int inum, char* name)
{
    if(entry == NULL || name == NULL || strlen(name) >= BUFS_DIRECOTRY_ENTRY_NAME_MAXLEN)
        return -1;
    
    memset(entry, 0, sizeof(bufs_dirent_t));
    entry->inum = inum;
    strcpy(entry->name, name);
    
    return 0;
}

void bufs_dir_ent_freearray(bufs_dirent_t** array_ptr)
{
    free(*array_ptr);
}

// Helper
static int bufs_dirent_readone(int fd, bufs_dirent_t* buf)
{ 
    int rc = read(fd, buf, sizeof(bufs_dirent_t));
    if(rc != sizeof(bufs_dirent_t))
        return -2;
    if(bufs_dirent_is_empty(buf))
        return -1; 
    else
        return 0;
}

static int bufs_dirent_find(int fd, unsigned int dir_db_addr,char* name, bufs_dirent_t* buf)
{ 
    bufs_dirent_t temp;
    int ent_idx; 

    ent_idx = 0;
    lseek(fd, dir_db_addr * BUFS_BLOCK_SIZE, SEEK_SET);
    while(bufs_dirent_readone(fd, &temp) == 0)
    {
        if(strcmp(temp.name, name) == 0) 
        {
            if(buf != NULL)
                *buf = temp;
            return ent_idx;
        }
        ++ent_idx;
    }
    return -1;
}

static unsigned int bufs_dirent_dbaddr(int fd, int dinum)
{ 
    bufs_super_block_t super;
    bufs_inode_t inode; 

    bufs_super_get(fd, &super);
    bufs_inode_get(fd, super.inode_region_address, dinum, &inode);
    return inode.direct[0];
}

static int bufs_dirent_is_empty(bufs_dirent_t* ent)
{
    char empty_entname[BUFS_DIRECOTRY_NAME_MAXLEN] = {0};
    return (ent->inum == 0 && 
            memcmp(ent->name, empty_entname, BUFS_DIRECOTRY_NAME_MAXLEN) == 0);
}