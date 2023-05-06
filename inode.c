#include "free.h"
#include "block.h"

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