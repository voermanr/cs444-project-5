#include "image.h"
#include <unistd.h>
#include "block.h"
#include "inode.h"

void mkfs(void) {
    unsigned char *block[BLOCK_SIZE]= {0};
    write(image_fd, block, 1024 * BLOCK_SIZE);
    for (int i = 0; i < 7; ++i) {
        alloc();
    }

    unsigned int inode_num = ialloc()->inode_num;
    unsigned int data_block_num = alloc();
}