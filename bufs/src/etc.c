#include"etc.h"
#include"param.h"

#include<unistd.h> 

//return 0 on success, -1 on failure(because of invalid input)
int byte_set(char* byte, int bit_idx, int val)
{ 
    char mask; 

    if(byte == NULL || bit_idx >= 8 || bit_idx < 0 || (val != 1 && val != 0) )
        return -1;

    if(val == 0)
    {
        mask = (0x1 << bit_idx) ^ 0xff;
        (*byte) = *byte & mask; 
    }
    else 
    {
        mask = (0x1 << bit_idx);
        *byte = *byte | mask;
    }
    return 0;
}

//return the index of the specific val in the byte, -1 if not found the val.
int byte_index(char byte, int val)
{
    int idx = 0, bits_len = 8; 
 
    if(val == 1)
    {
        for(idx=0 ; idx < bits_len && ( (0x01 & byte) != 0x01 ) ;++idx)
            byte >>= 1;
    }
    else if(val == 0)
    {
        for(idx = 0; idx < bits_len && ( (0x00 | (0x01&byte)) != 0x00 ); ++idx)
            byte >>= 1;
    }

    return idx == bits_len ? -1 : idx;
}

// return the val on the bit_idx : 0 or 1. If bit_idx invalid or byte is NULL, return -1
int byte_lookup(char* byte, int bit_idx)
{
    if(bit_idx < 0 || bit_idx > 7 || byte == NULL)
        return -1;
    
    char mask = (0x01 << bit_idx);

    return ((*byte) & mask) != 0;
}

__off_t seek_block(int fd, unsigned int block_address)
{
    return lseek(fd, block_address * BUFS_BLOCK_SIZE, SEEK_SET);
}

//return allocated corresponding index, -1 on failure. Just fit one block
int bitmap_alloc(int fd, bufs_addr_t bitmap_address, int bits_num)
{
    char byte;
    int base, idx, read_bits;

    lseek(fd, BUFS_BLOCK_SIZE * bitmap_address, SEEK_SET);
    if(read(fd, &byte, sizeof(char)) < 0 )
        return -1;
    read_bits = 8;
    base = 0;
    while( ( idx = byte_index(byte, 0) ) == -1)
    { 
        if(read(fd, &byte, sizeof(char)) < 0)
            return -1;
        read_bits += 8;
        base += 8; 
    }  
    if(read_bits > bits_num)
        return -1;

    byte_set(&byte, idx, 1);
    lseek(fd,lseek(fd, 0, SEEK_CUR) - sizeof(char), SEEK_SET);
    int written = write(fd, &byte, sizeof(char));
    if(written != sizeof(char))
        return -1;

    return base + idx;
}

//return 0 on success, -1 on failure(freed or system error). Just fit one block.
int bitmap_free(int fd, bufs_addr_t bitmap_address, int bit_idx)
{
    int byte_idx = bit_idx / 8, byte_bit_idx = bit_idx % 8; 
    char byte;

    lseek(fd, BUFS_BLOCK_SIZE * bitmap_address+byte_idx, SEEK_SET);
    int readbytes = read(fd, &byte, sizeof(char));
    if(readbytes != sizeof(char))
        return -1;

    if(byte_lookup(&byte, byte_bit_idx) != 1)
        return -1;

    int pos = lseek(fd, 0, SEEK_CUR) - sizeof(char);
    byte_set(&byte, byte_bit_idx, 0);
    lseek(fd, pos, SEEK_SET);
    int written = write(fd, &byte, sizeof(char));
    if(written != sizeof(char))
        return -1;

    return 0;
}

int bitmap_index_value(int fd, bufs_addr_t bitmap_address, int bit_idx)
{
    int byte_idx = bit_idx / 8, byte_bit_idx = bit_idx % 8; 
    char byte;

    lseek(fd, BUFS_BLOCK_SIZE * bitmap_address+byte_idx, SEEK_SET);
    int readbytes = read(fd, &byte, sizeof(char));
    if(readbytes != sizeof(char))
        return -1;

    return byte_lookup(&byte, byte_bit_idx);
}