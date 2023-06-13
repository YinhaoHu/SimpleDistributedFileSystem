#ifndef _BUFS_PARAM_H
#define _BUFS_PARAM_H

#define BUFS_DIRECOTRY (0)
#define BUFS_REGULAR_FILE (1)

#define BUFS_ISVALID_INODE_TYPE(itype) \
            ((itype == BUFS_DIRECOTRY) || (itype == BUFS_REGULAR_FILE))

#define BUFS_BLOCK_SIZE (4096)

#define BUFS_DIRECT_PTRS (30)

#define BUFS_DIRECOTRY_MAX_ENTRIES (8)
#define BUFS_DIRECOTRY_NAME_MAXLEN (28)

//Include '\0'
#define BUFS_DIRECOTRY_ENTRY_NAME_MAXLEN (28)

#endif