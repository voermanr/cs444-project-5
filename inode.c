#include "free.h"
#include "inode.h"
#include "pack.h"
#include "stdio.h"

#define INODE_OFFSET_SIZE 0
#define INODE_OFFSET_OWNER_ID 4
#define INODE_OFFSET_PERMISSIONS 6
#define INODE_OFFSET_FLAGS 7
#define INODE_OFFSET_LINK_COUNT 8
#define INODE_OFFSET_BLOCK_PTR_START 9
#define INODE_OFFSET_BLOCK_PTR_SPACING 2


// In-Core array and related functions
struct inode incore[MAX_SYS_OPEN_FILES] = {0};

void set_incore_inode(unsigned int incore_inode_position) {
    incore[incore_inode_position].ref_count++;
}

void set_incore_inode_with_size(unsigned int pos, unsigned int size) {
    set_incore_inode(pos);
    incore[pos].size = size;
}

void unset_incore_inode(unsigned int incore_inode_position) {
    incore[incore_inode_position].ref_count = 0;
}

struct inode *get_incore_inode_address(unsigned int pos) {
    return &incore[pos];
}

void set_incore_inode_and_inode_num(unsigned int pos, unsigned int inode_num) {
    incore[pos].inode_num = inode_num;
}


// public functions
struct inode *find_incore_free(void) {
    for (int i = 0; i < MAX_SYS_OPEN_FILES; ++i) {
        if (incore[i].ref_count == 0) {
            return &incore[i];
        }
    }
    return 0;
}

struct inode *find_incore(unsigned int inode_num) {
    for (int i = 0; i < MAX_SYS_OPEN_FILES; ++i) {
        if(incore[i].ref_count >= 1 && incore[i].inode_num == inode_num) {
            return &incore[i];
        }
    }
    return 0;
}

int ialloc(void) {
    unsigned char inode_map[BLOCK_SIZE] = {0};
    bread(BLOCK_INODE_MAP, inode_map);

    int return_value = find_free(inode_map);

    if (return_value != -1) {
        set_free(inode_map, return_value, 1);
        bwrite(BLOCK_INODE_MAP, inode_map);
    }

    return return_value;
}

void read_inode(struct inode *in, int inode_num) {
    int inode_block_number = inode_num / INODES_PER_BLOCK + BLOCK_INODE_DATA_BLOCK_0;
    int inode_block_offset = inode_num % INODES_PER_BLOCK;
    int inode_block_offset_bytes = inode_block_offset * INODE_SIZE;

    unsigned char block[BLOCK_SIZE] = {0};
    bread(inode_block_number, block);

    in->inode_num = inode_num;
    in->size = read_u32(block + inode_block_offset_bytes + INODE_OFFSET_SIZE);
    in->owner_id = read_u16(block + inode_block_offset_bytes + INODE_OFFSET_OWNER_ID);
    in->permissions = read_u8(block + inode_block_offset_bytes + INODE_OFFSET_PERMISSIONS);
    in->flags = read_u8( block + inode_block_offset_bytes + INODE_OFFSET_FLAGS);

    for (int i = 0; i < INODE_PTR_COUNT; ++i) {
        unsigned char *addr = block + inode_block_offset_bytes + INODE_OFFSET_BLOCK_PTR_START + i * INODE_OFFSET_BLOCK_PTR_SPACING;
        in->block_ptr[i] = read_u16(addr);
    }
}

void write_inode(struct inode *in) {
    int inode_num = in->inode_num;
    int inode_block_number = inode_num / INODES_PER_BLOCK + BLOCK_INODE_DATA_BLOCK_0;
    int inode_block_offset = inode_num % INODES_PER_BLOCK;
    int inode_block_offset_bytes = inode_block_offset * INODE_SIZE;

    unsigned char block[BLOCK_SIZE] = {0};
    bread(inode_block_number, block);

    write_u32(block + inode_block_offset_bytes + INODE_OFFSET_SIZE, in->size);
    write_u16(block + inode_block_offset_bytes + INODE_OFFSET_OWNER_ID, in->owner_id);
    write_u8(block + inode_block_offset_bytes + INODE_OFFSET_PERMISSIONS, in->permissions);
    write_u8( block + inode_block_offset_bytes + INODE_OFFSET_FLAGS, in->flags);

    for (int i = 0; i < INODE_PTR_COUNT; ++i) {
        unsigned char *addr = block + inode_block_offset_bytes + INODE_OFFSET_BLOCK_PTR_START + i * INODE_OFFSET_BLOCK_PTR_SPACING;
        write_u16(addr, in->block_ptr[i]);
    }

    bwrite(inode_block_number, block);
}

struct inode *iget(int inode_num) {
    struct inode *sussy_inode = find_incore(inode_num);

    if (sussy_inode) {
        (sussy_inode->ref_count)++;
        return sussy_inode;
    }

    sussy_inode = find_incore_free();
    if (!sussy_inode) {
        return 0;
    }

    read_inode(sussy_inode, inode_num);
    sussy_inode->ref_count = 1;
    sussy_inode->inode_num = inode_num;
    return sussy_inode;
}