#include "block.h"

int set_free(unsigned char *block, int num, int set) {
    int byte_num = num / 8;
    int bit_num = num % 8;

    if (set) {
        block[byte_num] |= (1 << bit_num);
    } else {
        block[byte_num] &= ~(1 << bit_num);
    }

    return 0;
}

int find_low_clear_bit(unsigned char x) {
    for (int i = 0; i < 8; i++)
        if (!(x & (1 << i)))
            return i;

    return -1;
}

int find_free(unsigned char *block) {
    for (int i = 0; i < BLOCK_SIZE; ++i) {
        int return_value = find_low_clear_bit(block[i]);
        if (return_value != -1) {
            return return_value;
        }
    }
    return -1;
}