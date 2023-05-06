#ifndef BLOCK_H
#define BLOCK_H

#define BLOCK_SUPERBLOCK 0
#define BLOCK_INODE_MAP 1
#define BLOCK_BLOCK_MAP 2
#define BLOCK_INODE_DATA_BLOCK_0 3
#define BLOCK_INODE_DATA_BLOCK_1 4
#define BLOCK_INODE_DATA_BLOCK_2 5
#define BLOCK_INODE_DATA_BLOCK_3 6
#define BLOCK_SIZE 4096

unsigned char *bread(int block_num, unsigned char *block);

void bwrite(int block_num, unsigned char *block);

int alloc();

#endif