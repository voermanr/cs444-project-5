#include <unistd.h>
#include "image.h"
#include "block.h"
#include "free.h"

int calculate_byte_num(int block_num) { return block_num * BLOCK_SIZE; }

unsigned char *bread(int block_num, unsigned char *block) {
    int byte_num = calculate_byte_num(block_num);

    lseek(image_fd, byte_num, SEEK_SET);
    read(image_fd, block, BLOCK_SIZE);

    return block;
}

void bwrite(int block_num, unsigned char *block) {
    int byte_num = calculate_byte_num(block_num);

    lseek(image_fd, byte_num, SEEK_SET);
    write(image_fd, block, BLOCK_SIZE);
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

