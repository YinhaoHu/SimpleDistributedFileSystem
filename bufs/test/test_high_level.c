#include "../src/mkfs.h"
#include "../src/dir.h"
#include "../src/etc.h"
#include "../user/user.h"

#include "test_framework.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <assert.h>
#include <unistd.h>

//test read(), write(), create(), unlink()
static void test_rwcu(bufs_fs_t* fs_info); 
//test list(), lookup(), stat()
static void test_lls(bufs_fs_t* fs_info);

int main(int argc, char* argv[])
{
    int rc;
    bufs_fs_t fs_info;
    
    if(argc != 2)
    {
        fprintf(stderr, "Usage:%s <img_file>\n", argv[0]);
        exit(-1);
    }
 
    rc = bufs_fs_mount(argv[1], &fs_info);
    assert(rc == 0);
    print_output_border(argv[0], 1);
    mkfs(fs_info.img_name);
    test_rwcu(&fs_info);
    test_lls(&fs_info);
    rc = bufs_fs_unmount(&fs_info);
    assert(rc == 0);
    print_output_border(argv[0],0);

    return 0;
}

static void test_rwcu(bufs_fs_t* fs_info)
{
    int create_rc, read_rc, write_rc, unlinc_rc, loop_rc;
    size_t rw_bytes, rw_count;
    char* read_buf, *write_buf, name[BUFS_DIRECOTRY_ENTRY_NAME_MAXLEN];

    memset(name, 0, BUFS_DIRECOTRY_ENTRY_NAME_MAXLEN);
    strcpy(name, "Test_File");
    // should be the second inode, the first is super.
    {
        create_rc = bufs_create(fs_info, 0,name, BUFS_REGULAR_FILE); 
        bufs_dirent_t* entries; 
        int s = bufs_dir_ent_array(fs_info->fd, 0,&entries); 
        print_result("test_create", create_rc == 1 && s == 3, __LINE__);
    }

    rw_count = 1024; 
    loop_rc = 0;read_rc = 0; write_rc = 0;

    rw_bytes = rw_count+1;
    write_buf = (char*)malloc(rw_bytes);
    memset(write_buf, 8, rw_bytes);
    write_rc = bufs_write(fs_info, create_rc, write_buf, 0, rw_bytes); 
    assert(write_rc == rw_bytes);
    free(write_buf);
    size_t cur_size = 0;
    cur_size = write_rc;
    for(size_t i = 0; i < rw_count; ++i)
    { 
        int offset = rand() % rw_count;
        rw_bytes = rand() % (BUFS_BLOCK_SIZE * 30);

        write_buf = (char*)malloc(rw_bytes);
        memset(write_buf, i, rw_bytes);
        write_rc = bufs_write(fs_info, create_rc, write_buf, offset, rw_bytes);  
        if(write_rc != rw_bytes && offset+rw_bytes >= cur_size) 
            continue;   
        assert(write_rc == rw_bytes);

        read_buf = (char*)malloc(rw_bytes);
        memset(read_buf, 0, rw_bytes);
        read_rc = bufs_read(fs_info, create_rc, read_buf, offset, rw_bytes); 
        assert(read_rc == rw_bytes);

        loop_rc += memcmp(read_buf, write_buf, rw_bytes);
        free(write_buf);
        free(read_buf);
        cur_size =MAX(offset+rw_bytes , cur_size);
    }     
    print_result("test_rw", (loop_rc == 0),__LINE__);
     
    {
        unlinc_rc = bufs_unlink(fs_info, 0, name); 
        bufs_dirent_t* entries;
        int size = bufs_dir_ent_array(fs_info->fd, 0,&entries);
        print_result("test_unlink", unlinc_rc == 0 && size == 2, __LINE__);
    }
 
}

static void test_lls(bufs_fs_t* fs_info)
{ 
    int pinum = 0;
    char *file_name = "test_file";
    bufs_stat_t stat; 

    //bufs_list()   : Test pass by print, then skip.

    //bufs_stat()
    bufs_stat(fs_info, 0, &stat);
    print_result("stat", stat.inum==0 && stat.size == (sizeof(bufs_dirent_t)*2) 
        && stat.type == BUFS_DIRECOTRY,__LINE__);
    int rc = bufs_stat(fs_info, 100, &stat);
    print_result("stat", rc < 0, __LINE__);
    //bufs_lookup
    {
        int cinum, linum, lurc;
        cinum=bufs_create(fs_info, pinum, file_name, BUFS_REGULAR_FILE);
        bufs_lookup(fs_info, pinum, file_name, &linum);

        bufs_unlink(fs_info,pinum, file_name);
        lurc = bufs_lookup(fs_info, pinum,file_name, NULL);
        print_result("lookup", linum == cinum && lurc == 0, __LINE__);
    }
    int newinum ,prc;
    
    newinum = bufs_create(fs_info, 0, "absnew", BUFS_DIRECOTRY);
    bufs_create(fs_info, newinum, "newfile", BUFS_REGULAR_FILE);
    prc = bufs_unlink(fs_info, 0, "absnew");  
    print_result("NewTest", prc == -6, __LINE__);
}