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

typedef enum file_type {
    UNKNOWN,
    REGULAR,
    DIRECTORY
} file_type;

void set_incore_inode(unsigned int pos, struct inode *in);
void set_incore_inode_with_size(unsigned int pos, unsigned int size);
void set_incore_inode_and_inode_num(unsigned int pos, unsigned int inode_num);
void unset_incore_inode(unsigned int pos);
struct inode *get_incore_inode_address(unsigned int pos);
void clear_incore_inodes(void);


// Useful functions
struct inode *find_incore_free(void);
struct inode *find_incore(unsigned int inode_num);
struct inode *ialloc(void);
void read_inode(struct inode *in, int inode_num);
void write_inode(struct  inode *in);
struct inode *iget(int inode_num);
void iput(struct inode *in);

#endif