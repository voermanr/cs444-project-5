#include "free.h"
#include "block.h"

#define INODE_OFFSET_SIZE 0
#define INODE_OFFSET_OWNER_ID 4
#define INODE_OFFSET_PERMISSIONS 6
#define INODE_OFFSET_FLAGS 7
#define INODE_OFFSET_LINK_COUNT
#define INODE_OFFSET_BLOCK_PTR_START 9
#define INODE_OFFSET_BLOCK_PTR_SPACING 2
#define INODE_NUM_BLOCK_PTR_BLOCKS 16

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