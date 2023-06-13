#include"datablock.h"
#include"etc.h"
#include"types.h"

// return the block address on success, -1 on failure because of space
bufs_addr_t bufs_db_alloc(int fd, bufs_addr_t bitmap_address, int num_data, bufs_addr_t db_region_addr)
{
    bufs_addr_t which_db = bitmap_alloc(fd, bitmap_address, num_data);

    return (which_db == -1) ? -1 : (which_db + db_region_addr);
}

// return 0 on success, -1 on failure because of invalid address
int bufs_db_free(int fd, bufs_addr_t bitmap_address,
    bufs_addr_t db_region_address ,bufs_addr_t db_address)
{
    return bitmap_free(fd, bitmap_address, db_address - db_region_address);
}

// return 0 on success, -1 on failure
// db_address is the block address, buf would be BUFS_BLOCK_SIZE bytes
int bufs_db_read(int fd, unsigned int db_address, char* buf)
{       
    seek_block(fd, db_address);
    int rc = read(fd, buf, BUFS_BLOCK_SIZE);
    return (rc == BUFS_BLOCK_SIZE) ? 0 : -1;
}

// return 0 on success, -1 on failure
// db_address is the block address.
int bufs_db_write(int fd, unsigned int  db_address, char*db)
{
    seek_block(fd, db_address);
    int rc = write(fd, db, BUFS_BLOCK_SIZE);
    return (rc == BUFS_BLOCK_SIZE) ? 0 : -1;
}