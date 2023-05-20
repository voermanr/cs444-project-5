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

unsigned int bitmap_position(int byte_num, int bit_pos) { return byte_num * 8 + bit_pos; }

int find_free(unsigned char *block) {
    for (int i = 0; i < BLOCK_SIZE; ++i) {
        int bit_pos = find_low_clear_bit(block[i]);
        if (bit_pos != -1) {
            return bitmap_position(i, bit_pos);
        }
    }
    return -1;
}