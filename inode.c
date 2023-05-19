#include "free.h"
#include "block.h"
#include "inode.h"
#include "pack.h"

#define INODE_SIZE 64
#define INODES_PER_BLOCK (BLOCK_SIZE / INODE_SIZE)

#define INODE_OFFSET_SIZE 0
#define INODE_OFFSET_OWNER_ID 4
#define INODE_OFFSET_PERMISSIONS 6
#define INODE_OFFSET_FLAGS 7
#define INODE_OFFSET_LINK_COUNT 8
#define INODE_OFFSET_BLOCK_PTR_START 9
#define INODE_OFFSET_BLOCK_PTR_SPACING 2


struct inode incore[MAX_SYS_OPEN_FILES] = {0};

struct inode *find_incore_free(void) {
    for (int i = 0; i < MAX_SYS_OPEN_FILES; ++i) {
        if (incore[i].link_count == 0) {
            return &incore[i];
        }
    }
    return 0;
}

struct inode *find_incore(unsigned int inode_num) {
    (void)inode_num;
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

    in->size = read_u32(block + inode_block_offset_bytes + INODE_OFFSET_SIZE);
    in->owner_id = read_u16(block + inode_block_offset_bytes + INODE_OFFSET_OWNER_ID);
    in->permissions = read_u8(block + inode_block_offset_bytes + INODE_OFFSET_PERMISSIONS);
    in->flags = read_u8( block + inode_block_offset_bytes + INODE_OFFSET_FLAGS);

    for (int i = 0; i < INODE_PTR_COUNT; ++i) {
        unsigned char *addr = block + inode_block_offset_bytes + INODE_OFFSET_BLOCK_PTR_START + i * INODE_OFFSET_BLOCK_PTR_SPACING;
        in->block_ptr[i] = read_u16(addr);
    }
}

struct inode *iget(int inode_num) {
    //TODO return in-core inode for inode_num
    struct inode *sussy_inode = find_incore(inode_num);
    if (sussy_inode) {
        (sussy_inode->ref_count)++;
        return sussy_inode;
    }

    sussy_inode = find_incore_free();
    if (sussy_inode) {
        read_inode(sussy_inode, inode_num);
        //TODO complete implementation
    }

    return 0;
}