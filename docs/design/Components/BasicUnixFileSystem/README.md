# Basic Unix File System

---

# Introduction

  I'll call basic unix file system as BUFS for convenience in the latter text. As you can see later, the fixed specifications are not nice because of the latent waste(I did this for simplicity).  Althouth there are something unreasonable,  just finish the core functions first, optimization is the next thing to think about, right? 

    The storage layout in the BUFS looks like:

```
--------------------------------------------------------------------------
|  SuperBlock  |  Inode Bitmap | Data Bitmap  |  Inodes  |  DataBlocks   |
--------------------------------------------------------------------------
```

    Each component plays different roles

```
SuperBlock: stores the address and length of InodeBitmap, DataBitmap,Inodes,
DataBlocks. Plus the number of data blocks and inode blocks.

InodeBitmap: indicates the usgae state of each inode block.
DataBitmap: indecates the usage state of each data block.

Inodes: stores the associated data block addresses of the block.
DataBlocks： stores the data.
```

    The specifications are shown as following

```
Each function call is not thread-safe, so use mutex lock for safety in the 
multi-threads program.

Basis:
    IOBlockSize = 4096 bytes 
    MAX_FILES = 24
    MAX_DIRS = 8
Storage:
    Data Blocks for files = 30 * 28
    Data Blocks for dirs =   1 * 4

Inode:
    InodeSize = 128 bytes
    InodeBlock = 1 block 
    MaxInodes = 32 inodes

Block:
    SuperBlock = 1 block                Address: 0
    InodeBitmap = 1 block               Address: 1
    DataBitmap = 1 block                Address: 2
    InodeBlock = 1 block                Address: 3
    DataBlocks = 728 blocks             Address: 4 ~ 731
    TotalBlocks = 732 blocks
```

# Usage

Run the mkfs
