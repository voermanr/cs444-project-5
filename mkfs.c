#include "image.h"
#include <unistd.h>
#include "block.h"
#include "inode.h"
#include "stdio.h"
#include "pack.h"
#include "string.h"
#include <errno.h>

#define DIRECTORY_ENTRY_SIZE 32
#define DIRECTORY_DEFAULT_ENTRIES 2

typedef enum file_type {
    UNKNOWN,
    REGULAR,
    DIRECTORY
} file_type;

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

    unsigned int inode_num = ialloc()->inode_num;
    unsigned short data_block_num = alloc();

    struct inode vessel;
    vessel.ref_count = 1;
    vessel.inode_num = inode_num;
    vessel.flags = DIRECTORY;
    vessel.size = DIRECTORY_ENTRY_SIZE * DIRECTORY_DEFAULT_ENTRIES;
    vessel.block_ptr[0] = data_block_num;

    unsigned char inode_data_block_0[BLOCK_SIZE] = {0};
    unsigned char *offset_ptr = inode_data_block_0;
    write_u16(offset_ptr, 0);
    offset_ptr += 2;
    strcpy((char *)(offset_ptr), ".\0");
    offset_ptr +=16;
    offset_ptr +=14;
    write_u16(offset_ptr, 0);
    offset_ptr += 2;
    strcpy((char *)(offset_ptr), "..\0");

    bwrite(data_block_num, inode_data_block_0);

    iput(&vessel);

}

// int main(void) {
    // image_open("garbage.vvsfs",1);
    // mkfs();
    // return 0;
// }