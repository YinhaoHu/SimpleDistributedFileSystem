/*!
  Example1: This example will walk you through the through and basic
    usage of SDFS API with one client. For simplicity, there is no
    error checking. Hence, you need tomake the bufs_img file make_new in the
    server side. Or, you might get unexpected results here. For clearly
    explaination, there might be some verbose code lines which should be
    wrapped.
*/

#include "sdfs.h"
#include "snftp_errno.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>

#define STR_TYPE(type) (((type) == SDFS_DIRECOTRY) ? "Direcotry":"Regular File")

static char *server_host, *server_service;
static void check_option(char* prog, int argc, char* argv[]);
static void make_new(char** ptr, sdfs_size_t nbytes);
static void safe_init();
static void print_rwbuf(char*part, char *read_buf, char *write_buf);
static void print_stat(sdfs_stat_t* sbuf);


int main(int argc, char* argv[])
{
    char *dir_name,*file1_name, *file2_name;
    char *file1_read_buf, *file1_write_buf;
    char *file2_read_buf, *file2_write_buf;
    sdfs_int_t dir_inum, file1_inum, file2_inum, temp, list_size;
    sdfs_size_t file1_buf_size, file2_buf_size;
    sdfs_stat_t sbuf, *sbuf_list;

    check_option(argv[0], argc, argv);
    printf("\033[1;37m\tAttention: Ensure that the server's bufs_img is newed.\033[0m\n");
    safe_init();                                                            // SDFS CODE LINE(Implicit)
    dir_name = "make_new_dir";
    file1_name = "make_new_file1";
    file2_name = "make_new_file2";

    dir_inum = sdfs_create(SDFS_ROOT_INUM, dir_name, SDFS_DIRECOTRY);       // SDFS CODE LINE
    file1_inum = sdfs_create(dir_inum, file1_name, SDFS_REGULAR_FILE);      // SDFS CODE LINE
    file2_inum = sdfs_create(dir_inum, file2_name, SDFS_REGULAR_FILE);      // SDFS CODE LINE

    printf("Inums: dir_inum = %d | file1_inum = %d | file2_inum = %d\n\n",
        dir_inum, file1_inum, file2_inum);

    file1_buf_size = 64;
    make_new(&file1_read_buf, file1_buf_size);
    make_new(&file1_write_buf,file1_buf_size);
    strcpy(file1_write_buf, "Hello, world!");

    sdfs_write(file1_inum, file1_write_buf, 0, file1_buf_size);             // SDFS CODE LINE
    sdfs_read(file1_inum, file1_read_buf, 0, file1_buf_size);               // SDFS CODE LINE
    print_rwbuf("file1", file1_read_buf, file1_write_buf); 


    file2_buf_size = 4100;
    make_new(&file2_read_buf, file2_buf_size);
    make_new(&file2_write_buf, file2_buf_size);
    strcpy(file2_write_buf, "Programming is always wonderful!");

    sdfs_write(file2_inum, file2_write_buf, 0, file2_buf_size);             // SDFS CODE LINE
    sdfs_read(file2_inum, file2_read_buf, 0, file2_buf_size);               // SDFS CODE LINE
    print_rwbuf("file2", file2_read_buf,file2_write_buf); 

    sdfs_stat(file1_inum, &sbuf);                                           // SDFS CODE LINE
    print_stat(&sbuf);

    temp = sdfs_lookup(dir_inum, file1_name);                               // SDFS CODE LINE
    printf("Result of lookup: %d.\n", temp);

    putchar('\n');
    printf("Direnctory entries in the direcotry whose inum = %d \n", dir_inum);
    list_size = sdfs_list(dir_inum, &sbuf_list);                            // SDFS CODE LINE
    for(sdfs_int_t idx = 0; idx < list_size; ++idx)
        print_stat(sbuf_list+idx);
    sdfs_freelist(&sbuf_list);                                              // SDFS CODE LINE                      

    putchar('\n');
    sdfs_unlink(dir_inum, file1_name);                                      // SDFS CODE LINE 
    
    printf("Direnctory entries in the direcotry whose inum = %d \n", dir_inum);
    list_size = sdfs_list(dir_inum, &sbuf_list);                            // SDFS CODE LINE 
    for(sdfs_int_t idx = 0; idx < list_size; ++idx)
        print_stat(sbuf_list+idx);
    sdfs_freelist(&sbuf_list);                                              // SDFS CODE LINE 


    putchar('\n');
    printf("Direnctory entries in the direcotry whose inum = %d \n", SDFS_ROOT_INUM);
    list_size = sdfs_list(0, &sbuf_list);                                   // SDFS CODE LINE 
    for(sdfs_int_t idx = 0; idx < list_size; ++idx)
        print_stat(sbuf_list+idx);
    sdfs_freelist(&sbuf_list);  


    free(file1_read_buf);
    free(file1_write_buf);
    free(file2_read_buf);
    free(file2_write_buf);

    exit(EXIT_SUCCESS);
}

static void print_stat(sdfs_stat_t* sbuf)
{
    printf("Inode number: %-3d | Size = %-6d bytes | Type = %s\n",
        sbuf->inum, sbuf->size, STR_TYPE(sbuf->type));
}

static void print_rwbuf(char*part, char *read_buf, char *write_buf)
{
    printf("Buffers of %s\n\tRead_buf: %s | Write_buf: %s\n", 
        part, read_buf, write_buf);
}

static void make_new(char** ptr, sdfs_size_t nbytes)
{
    *ptr = (char*)malloc(nbytes);
    memset(*ptr, 0, nbytes);
}

static void check_option(char* prog, int argc, char* argv[])
{
    if(argc != 3)
    {
        fprintf(stderr, "Usage: %s <host> <service>\n", prog);
        exit(EXIT_FAILURE);
    }

    server_host = argv[1];
    server_service = argv[2];
}

static void safe_init()
{
    sdfs_return_code_t rc;

    rc = sdfs_init(server_host, server_service);
    if(rc.status != SDFS_RC_STATUS_OK)
    {
        fprintf(stderr, "Error: %s\n", rc.text);
        exit(EXIT_FAILURE);
    }
}
