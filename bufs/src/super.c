#include "super.h"

#include <malloc.h>
#include <unistd.h>
#include <string.h>

// return 0 on success, -1 on failure.
int bufs_super_get(int fd, bufs_super_block_t *buf)
{
    size_t size;
    int rc;

    size = sizeof(bufs_super_block_t);
    lseek(fd, 0, SEEK_SET);
    rc = read(fd, buf, size);

    return rc == size ? 0 : -1;
}