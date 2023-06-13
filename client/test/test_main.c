#include "core/sdfs.h"
#include "snftp/snftp.h"
#include "snftp/snftp_errno.h"
#include "test_framework.h"
#include <stdlib.h>
#include <stdio.h> 
#include <malloc.h>
#include <string.h>
#include <sys/socket.h> 
#include <unistd.h>
#include <time.h>
 
#define TEST_MODES_NUM (2) 
static void init_get_option(int argc, char*argv[]);

static void test_manual();
static void test_lookup();
static void test_stat();
static void test_create();
static void test_list();
static void test_unlink();
static void test_write_read();

static char *mode;
static char* modes[] = {"auto", "manual"};
static char *hostname, *service;


int main(int argc, char* argv[])
{
    sdfs_return_code_t rc;

    init_get_option(argc, argv);

    rc = sdfs_init(hostname, service); 
    if(rc.status == SDFS_RC_STATUS_BAD)
    {
        fprintf(stderr, "Fail in initing: %s\n", rc.text);
        exit(EXIT_FAILURE);
    }
    print_output_border(argv[0], 1);
    if(strcmp(mode, modes[0]) == 0)
    {
        test_lookup();
        test_stat();
        test_create();
        test_list();
        test_unlink();
        test_write_read();
    }
    else
    {
        test_manual();
    }
    print_output_border(argv[0], 0); 

    return 0;
}

static void init_get_option(int argc, char*argv[])
{
    int idx; 

    if(argc != 4)
    {
        fprintf(stderr, "Usage: %s <hostname> <port> <mode>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    mode = argv[3];
    for(idx = 0; idx < TEST_MODES_NUM; ++idx)
    {
        if(strcmp(modes[idx], mode) == 0)
        {
            printf("\033[1;37mCurrent test mode: %s\033[0m\n", mode);
            break;
        }
    }
    if(idx == TEST_MODES_NUM)
    {
        printf("Error mode, available test modes: \n");
        for(idx = 0; idx<TEST_MODES_NUM; ++idx)
            printf("%d: %s\n", idx, modes[idx]);
        exit(EXIT_FAILURE);
    }
    
    hostname = argv[1];
    service = argv[2];
    printf("\033[1;33mTips: Make sure the server was rebuilt before test client.\033[0m\n");
    printf("\033[1;33mOr some cases might fail.\033[0m\n");
}

static void test_lookup()
{  
    sdfs_uint_t pinum_args[5] = {0,0,0,0,15};
    char* name_args[5] = {".", "..", "test", "NON","yes"};
    sdfs_uint_t results[5] = {0,0,SDFS_LOOKUP_NOT_FOUND,
        SDFS_LOOKUP_NOT_FOUND, SDFS_LOOKUP_PINUM_EXIST};
    
    for(sdfs_uint_t i = 0; i < 5; ++i)
    {
        print_result("lookup", sdfs_lookup(pinum_args[i], name_args[i]) == results[i],
             __LINE__); 
    }
}

static void test_stat()
{ 
    sdfs_stat_t stat_buf;
    
    sdfs_uint_t inum = 1;
    sdfs_stat_t buf;
    sdfs_int_t result = sdfs_stat(inum, &buf);
    print_result("sdfs_stat", result == SDFS_STAT_INUM, __LINE__);
    print_result("Stat", sdfs_stat(0, &stat_buf) == SDFS_OK && stat_buf.inum == 0, __LINE__);
}

static void test_create()
{
    #define cases_num (3) 

    sdfs_uint_t pinums[cases_num] = {SDFS_ROOT_INUM, SDFS_ROOT_INUM, 2};
    char* names[cases_num] = {"new_file_", "new_dir", "sub_file"};
    sdfs_int_t types[cases_num] = {SDFS_REGULAR_FILE, SDFS_DIRECOTRY,SDFS_REGULAR_FILE};
    sdfs_uint_t expect[cases_num] = {1,2,3};

    for(sdfs_uint_t idx = 0; idx < cases_num; ++idx)
    {
        sdfs_int_t rc =  sdfs_create(pinums[idx], names[idx], types[idx]);
        print_result("create", rc == expect[idx], __LINE__);
    }

    #undef cases_num
}

static void test_list()
{
    static int target = 4;
    sdfs_size_t size;
    sdfs_stat_t* buf = NULL;

    size = sdfs_list(SDFS_ROOT_INUM, &buf);  
    print_result("list", size == (target--), __LINE__);
 
    if(buf != NULL)
        sdfs_freelist(&buf);
}

static void test_unlink()
{
    print_result("unlink", sdfs_unlink(SDFS_ROOT_INUM, "new_file_") ==SDFS_OK, __LINE__);
    test_list();
}

static void test_write_read()
{
    char write_buf[64] = "Hello, world!", read_buf[64] = {0};
    char fake_buf[16] = {0}; 
 
    print_result("write", sdfs_write(SDFS_ROOT_INUM, fake_buf, 0, 16) == SDFS_WRITE_INUM_TYPE 
        , __LINE__);
 
    print_result("write", sdfs_write(3, fake_buf, 1, 16) == SDFS_WRITE_OFFSET, __LINE__);
 
    print_result("write", sdfs_write(3, write_buf, 0, 64) == 64, __LINE__);
 
    print_result("read", (sdfs_read(3, read_buf, 0, 64) == 64 )&& (memcmp(write_buf, read_buf, 64) == 0)
        , __LINE__); 

    print_result("read", (sdfs_read(3, read_buf, 0, 64) == 64 )&& (memcmp(write_buf, read_buf, 64) == 0)
        , __LINE__);

}

static void test_manual()
{
    /*
    
        If there is anyone interested in testing manually,
        write code here.

    */
} 

