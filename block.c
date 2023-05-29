#include <unistd.h>
#include "image.h"
#include "block.h"
#include "free.h"
#include <stdio.h>
#include <errno.h>
#include <string.h>

int calculate_byte_num(int block_num) { return block_num * BLOCK_SIZE; }

unsigned char *bread(int block_num, unsigned char *block) {
    int byte_num = calculate_byte_num(block_num);

    lseek(image_fd, byte_num, SEEK_SET);
    read(image_fd, block, BLOCK_SIZE);

    return block;
}

int bwrite(int block_num, unsigned char *block) {
    int byte_num = calculate_byte_num(block_num);
    //printf("bwrite(): \tblock_num: %d \tbyte_num: %d\n", block_num, byte_num);
    if (lseek(image_fd, byte_num, SEEK_SET) == -1) {
        printf("lseek error: %s\n", strerror(errno));
        return -1;
    }

    //printf("bwrite(): \timage_fd: %d\n", image_fd);
    int write_bytes = write(image_fd, block, BLOCK_SIZE);
    if (write_bytes == -1) {
        printf("bwrite(): \tWrite error: %s\n", strerror(errno));
        return -1;
    }

    return write_bytes;
}

int alloc(void) {
    unsigned char block_map[BLOCK_SIZE] = {0};
    bread(BLOCK_BLOCK_MAP, block_map);

    int return_value = find_free(block_map);

    if (return_value != -1) {
        set_free(block_map, return_value, 1);
        bwrite(BLOCK_BLOCK_MAP, block_map);
    }

    return return_value;
}

