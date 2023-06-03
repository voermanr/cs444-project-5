#include "image.h"
#include <unistd.h>
#include "block.h"
#include "inode.h"
#include "stdio.h"
#include "pack.h"
#include "string.h"
#include "directory.h"
#include <errno.h>

void mkfs(void) {
    unsigned char block[BLOCK_SIZE]= {0};
    for (int i = 0; i < 1024; ++i) {
        if (write(image_fd, block, BLOCK_SIZE) == -1) {
            printf("mkfs(): \tWrite error: %s\n", strerror(errno));
            return;
        }
    }

    for (int i = 0; i < 7; ++i) {
        alloc();
    }

    struct inode *inode_ptr = ialloc();
    unsigned short data_block_num = alloc();

    inode_ptr->ref_count = 1;
    inode_ptr->inode_num = 0;
    inode_ptr->flags = DIRECTORY;
    inode_ptr->size = DIRECTORY_ENTRY_SIZE_ON_DISK * DIRECTORY_DEFAULT_ENTRIES;
    inode_ptr->block_ptr[0] = data_block_num;

    unsigned char inode_data_block_0[BLOCK_SIZE] = {0};
    unsigned char *offset_ptr = inode_data_block_0;
    write_u16(offset_ptr, 0);
    offset_ptr += 2;
    strcpy((char *)(offset_ptr), ".");
    offset_ptr +=16;
    offset_ptr +=14;
    write_u16(offset_ptr, 0);
    offset_ptr += 2;
    strcpy((char *)(offset_ptr), "..");

    bwrite(data_block_num, inode_data_block_0);

    iput(inode_ptr);

}

/* int main(void) {
    image_open("garbage.vvsfs",1);
    mkfs();
    return 0;
} */