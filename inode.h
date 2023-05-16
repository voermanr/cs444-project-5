#ifndef INODE_H
#define INODE_H

#define INODE_PTR_COUNT 16
#define MAX_SYS_OPEN_FILES 64

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

extern struct inode incore[MAX_SYS_OPEN_FILES];

struct inode *find_incore_free(void);

int ialloc(void);

#endif