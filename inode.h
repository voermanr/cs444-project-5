#ifndef INODE_H
#define INODE_H

#include "block.h"

#define INODE_PTR_COUNT 16
#define MAX_SYS_OPEN_FILES 64

#define INODE_SIZE 64
#define INODES_PER_BLOCK (BLOCK_SIZE / INODE_SIZE)

struct inode {
    unsigned int size;
    unsigned short owner_id;
    unsigned char permissions;
    unsigned char flags;
    unsigned char link_count;
    unsigned short block_ptr[INODE_PTR_COUNT];

    // Below attributes only in-core
    unsigned int ref_count;
    unsigned int inode_num;
};

extern struct inode incore[];

struct inode *find_incore_free(void);
struct inode *find_incore(unsigned int inode_num);

int ialloc(void);

void read_inode(struct inode *in, int inode_num);
void write_inode(struct  inode *in);

#endif