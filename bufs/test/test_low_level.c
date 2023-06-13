#include<stdio.h>
#include<unistd.h>
#include<fcntl.h>
#include<assert.h>
#include<string.h>
#include<malloc.h>

#include "test_framework.h"
#include"../src/inode.h"
#include"../src/datablock.h"
#include"../src/dir.h"
#include"../src/mkfs.h"
#include"../user/user.h"

static void test_inode(int fd);
static void test_db(int fd);
static void test_dir(int fd);
static void test_mkfs(char* img_name);

int main(int argc, char* argv[])
{
	int rc;
	bufs_fs_t fs_info;

	if(argc != 2)
	{
		fprintf(stderr, "Usage:%s <img>\n",argv[0]);
		_exit(-1);
	}
	print_output_border(argv[0],1);

	//  Test Begin
	// 	After each part of tests, we set the fs the initialized status
	//	Initialized Status: superblock set, inode = 0 set, first db set.
  	test_mkfs(argv[1]);
	rc = bufs_fs_mount(argv[1], &fs_info);
	assert(rc == 0);
	test_inode(fs_info.fd);
	test_db(fs_info.fd);
	test_dir(fs_info.fd);
	rc = bufs_fs_unmount(&fs_info);
	assert(rc == 0);
	//	Test End
	print_output_border(argv[0],0);
 
    return 0;
}

static void test_inode(int fd)
{
	const int inode_num = BUFS_NUM_INODES;

	//test free and alloc
	// alloc 31 times first and free one then alloc once again.
	{
		int rc, loop_rc;
		 
		loop_rc = (rc = 0);
		for(int i = 1 ; i < inode_num; ++i)
		{
			rc += bufs_inode_alloc(fd, BUFS_INODE_BITMAP_ADDRESS, BUFS_NUM_INODES);
			loop_rc += i;
		}
		print_result("inode", rc == loop_rc, __LINE__);

		rc = bufs_inode_alloc(fd, BUFS_INODE_BITMAP_ADDRESS, BUFS_NUM_INODES);
		print_result("inode", rc == -1, __LINE__);

		rc = bufs_inode_free(fd, BUFS_INODE_BITMAP_ADDRESS, 31);
		print_result("inode", rc == 0, __LINE__);

		rc = bufs_inode_alloc(fd, BUFS_INODE_BITMAP_ADDRESS, BUFS_NUM_INODES);
		print_result("inode", rc == 31, __LINE__);

		loop_rc = (rc = 0);
		for(int inum = 31 ; inum != 0; --inum)
		{
			rc += bufs_inode_free(fd, BUFS_INODE_BITMAP_ADDRESS, inum);
			loop_rc += 0;
		} 
		print_result("inode", rc == loop_rc, __LINE__); 
	}

	//test set and get
	{
		int inum, rc;
		bufs_inode_t inode, temp;

		for(int i =0; i < BUFS_DIRECT_PTRS; ++i)
			inode.direct[i] = i;
		inode.type = BUFS_DIRECOTRY;
		inode.size = 12345;

		inum = bufs_inode_alloc(fd, BUFS_INODE_BITMAP_ADDRESS, BUFS_NUM_INODES); 
		rc = bufs_inode_set(fd, BUFS_INODE_REGION_ADDRESS, inum, &inode); 
		print_result("inode", rc == 0 && inum == 1, __LINE__);
		print_result("inode_exist", bufs_inode_exist(fd, BUFS_INODE_BITMAP_ADDRESS, inum) == 1,
			 __LINE__);

		rc = bufs_inode_get(fd, BUFS_INODE_REGION_ADDRESS, inum, &temp);
		print_result("inode", rc == 0, __LINE__);

		rc = memcmp(&inode, &temp, sizeof(bufs_inode_t));
		print_result("inode", rc == 0, __LINE__);
	}
	//test dir init
	{
		int rc;
		bufs_inode_t super_inode, buffer;

		memset(&super_inode, 0, sizeof(bufs_inode_t));
		super_inode.direct[0] = 4;
		super_inode.type = BUFS_DIRECOTRY;
		super_inode.size = sizeof(bufs_dirent_t) * 2;

		bufs_inode_get(fd, BUFS_INODE_REGION_ADDRESS, 0, &buffer);
		rc = memcmp(&super_inode, &buffer, sizeof(bufs_inode_t)); 

		print_result("inode", rc == 0, __LINE__);
	}

}

static void test_db(int fd)
{ 
	// Allocate and Free
	{
		int rc, loop_rc;

		rc = 0, loop_rc = 0; 
		for(int i = 1; i < BUFS_NUM_DATA; ++i)
		{
			rc += bufs_db_alloc(fd, BUFS_DATA_BITMAP_ADDRESS, BUFS_NUM_DATA, 
				BUFS_DATA_REGION_ADDRESS);
			loop_rc += i + BUFS_DATA_REGION_ADDRESS;
		}
		print_result("datablock-alloc", loop_rc == rc, __LINE__);

		// no space, should failure.
		rc = bufs_db_alloc(fd, BUFS_DATA_BITMAP_ADDRESS, BUFS_NUM_DATA, 
				BUFS_DATA_REGION_ADDRESS);   
		print_result("datablock-alloc", rc == -1, __LINE__);

		bufs_addr_t last_db_addr = (BUFS_NUM_DATA-1) + BUFS_DATA_REGION_ADDRESS;
		// free and alloc, should last block
		rc = bufs_db_free(fd, BUFS_DATA_BITMAP_ADDRESS, BUFS_DATA_REGION_ADDRESS, 
			last_db_addr);
		print_result("datablock-free", rc == 0, __LINE__);
		rc = bufs_db_alloc(fd, BUFS_DATA_BITMAP_ADDRESS, BUFS_NUM_DATA, 
				BUFS_DATA_REGION_ADDRESS);
		print_result("datablock-free", rc == last_db_addr, __LINE__);

		loop_rc = 0, rc = 0;

		for(bufs_addr_t db_addr = last_db_addr ; db_addr >= BUFS_DATA_REGION_ADDRESS +1; --db_addr)
		{
			rc += bufs_db_free(fd, BUFS_DATA_BITMAP_ADDRESS, BUFS_DATA_REGION_ADDRESS, 
			db_addr);
		}
		print_result("datablock-free", rc == loop_rc,  __LINE__);
	}
	// Read and Write
	{
		int rc;
		bufs_addr_t db_addr;
		char* write_buf, *read_buf;

		write_buf = (char*)malloc(BUFS_BLOCK_SIZE);
		read_buf = (char*)malloc(BUFS_BLOCK_SIZE);

		write_buf[0] = 0xff;
		write_buf[1] = 0x23;
		write_buf[2] = 0x01;

		memset(write_buf, 0, BUFS_BLOCK_SIZE);
		memset(read_buf, 0, BUFS_BLOCK_SIZE);

		rc = 0;
		db_addr = bufs_db_alloc(fd, BUFS_DATA_BITMAP_ADDRESS, BUFS_NUM_DATA, 
				BUFS_DATA_REGION_ADDRESS);

		rc += bufs_db_write(fd, db_addr, write_buf);
		rc += bufs_db_read(fd, db_addr, read_buf);

		rc += bufs_db_free(fd, BUFS_DATA_BITMAP_ADDRESS, BUFS_DATA_REGION_ADDRESS, 
			db_addr);

		print_result("datablock-read_write", 
			(memcmp(read_buf,write_buf,BUFS_BLOCK_SIZE)==0) && rc == 0 
			&& (db_addr == BUFS_DATA_REGION_ADDRESS + 1),__LINE__);

		free(write_buf);
		free(read_buf);
	}
}

static void test_dir(int fd)
{ 
	int inum, dinum, rc;
	char name[BUFS_DIRECOTRY_NAME_MAXLEN] = "TEST_NAME"; 
	bufs_dirent_t ent;

	inum = 19;
	dinum = 0; 
 
	rc = bufs_dir_ent_init(&ent, inum, name);
	rc += bufs_dir_ent_add(fd, dinum, &ent);
	print_result("dir_ent_add", rc == 0, __LINE__);

	rc = bufs_dir_ent_inum(fd, dinum, name);  
	print_result("dir_ent_inum",rc == inum,__LINE__);
	 
	{
		//teset array
		bufs_dirent_t *array;
		int array_size, cmp;
		bufs_dirent_t first_ent, second_ent;

		memset(&first_ent, 0, sizeof(bufs_dirent_t));
		memset(&second_ent, 0, sizeof(bufs_dirent_t));

		first_ent.inum = (second_ent.inum = 0);
		strcpy(first_ent.name, ".");
		strcpy(second_ent.name, "..");

		array_size = bufs_dir_ent_array(fd, dinum, &array);

		cmp = 0;
		cmp += memcmp(&first_ent, array+0, sizeof(bufs_dirent_t));
		cmp += memcmp(&second_ent, array+1, sizeof(bufs_dirent_t));
		cmp += memcmp(&ent, array+2, sizeof(bufs_dirent_t));

		print_result("dir_ent_array",(array_size==3) && (cmp == 0 ),__LINE__);

		bufs_dir_ent_freearray(&array);
	} 

	{
		int array_size;
		bufs_dirent_t *array;

		rc = bufs_dir_ent_del(fd, dinum, name);
		array_size = bufs_dir_ent_array(fd, dinum, &array);
 
		print_result("dir_ent_del", (array_size == 2), __LINE__);
		bufs_dir_ent_freearray(&array);
	}
}

static void test_mkfs(char* img_name)
{
 	int rc;
	
    rc = mkfs(img_name);
	print_result("mkfs", rc == 0, __LINE__);
}

