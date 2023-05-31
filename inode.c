#include "free.h"
#include "inode.h"
#include "pack.h"
#include "stdio.h"
#include "mkfs.h"
#include "image.h"
#include <string.h>

#define INODE_OFFSET_SIZE 0
#define INODE_OFFSET_OWNER_ID 4
#define INODE_OFFSET_PERMISSIONS 6
#define INODE_OFFSET_FLAGS 7
#define INODE_OFFSET_LINK_COUNT 8
#define INODE_OFFSET_BLOCK_PTR_START 9
#define INODE_OFFSET_BLOCK_PTR_SPACING 2


// In-Core array and related functions
struct inode incore[MAX_SYS_OPEN_FILES] = {0};

void set_incore_inode(unsigned int pos, struct inode *in) {
    struct inode *target = &incore[pos];
    target->ref_count = 1;
    target->inode_num = in->inode_num;
    target->size = in->size;
    target->flags = in->flags;
    target->permissions = in->permissions;
    target->owner_id = in->owner_id;
    //TODO target->block_ptr = some for loop
    target->link_count = in->link_count;
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

void set_default_inode(struct inode *in, unsigned int inode_num) {
    in->size = 0;
    in->owner_id = 0;
    in->permissions = 0;
    in->flags = 0;

    for (int i = 0; i < INODE_PTR_COUNT; ++i) {
        in->block_ptr[i] = 0;
    }

    in->inode_num = inode_num;
}

void clear_incore_inodes(void){
    memset(incore, 0, sizeof incore);
}


// public functions
struct inode *find_incore_free(void) {
    for (int i = 0; i < MAX_SYS_OPEN_FILES; ++i) {
        if (incore[i].ref_count == 0) {
            return &incore[i];
        }
    }
    return NULL;
}

struct inode *find_incore(unsigned int inode_num) {
    for (int i = 0; i < MAX_SYS_OPEN_FILES; ++i) {
        if(incore[i].ref_count >= 1 && incore[i].inode_num == inode_num) {
            return &incore[i];
        }
    }
    return NULL;
}

struct inode *ialloc(void) {
    unsigned char inode_map[BLOCK_SIZE] = {0};
    bread(BLOCK_INODE_MAP, inode_map);

    int inode_index = find_free(inode_map);

    if (inode_index != -1) {
        set_free(inode_map, inode_index, 1);
        bwrite(BLOCK_INODE_MAP, inode_map);
    } else { return NULL; }

    struct inode *incore_inode = iget(inode_index);
    if (!incore_inode) {
        return NULL;
    }

    set_default_inode(incore_inode, inode_index);
    write_inode(incore_inode);

    return incore_inode;
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
    //printf("iget(): trying to find inode number %d..", inode_num);
    struct inode *sussy_inode = find_incore(inode_num);

    if (sussy_inode) {
        printf("Found inode incore already.\n");
        (sussy_inode->ref_count)++;
        return sussy_inode;
    }

    sussy_inode = find_incore_free();
    if (!sussy_inode) {
        //printf("Didn't find any free incore slots!\n");
        return NULL;
    }

    read_inode(sussy_inode, inode_num);
    sussy_inode->ref_count = 1;
    sussy_inode->inode_num = inode_num;
    //printf("\n\tloaded num: %d, size: %d\n", sussy_inode->inode_num, sussy_inode->size);
    return sussy_inode;
}

void iput (struct inode *in) {
    if (in->ref_count == 0) {
        return;
    }
    in->ref_count--;
    if(in->ref_count == 0) {
        //printf("iput(): wrote inode %p, num: %d, size: %d\n", &in, in->inode_num, in->size);
        write_inode(in);
    }
}

//int main(void) {
//    image_open("trash",1);
//    mkfs();
//    iget(0);
//}