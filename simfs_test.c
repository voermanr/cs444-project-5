#include "ctest.h"
#include "image.h"
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include "block.h"
#include "free.h"
#include "inode.h"
#include "mkfs.h"

void test_image_open_creates_file() {
    char *filename = "test.vvsfs";

    CTEST_ASSERT(image_open(filename, 0) == 0, "file doesn't exist");

    remove(filename);
}

void test_image_open_without_truncate() {
    char *filename = "test.vvsfs";
    open(filename, O_RDWR | O_CREAT);

    CTEST_ASSERT(image_open(filename, 0) == 0, "without truncate");

    remove(filename);
}

void test_image_open_with_truncate() {
    char *filename = "test.vvsfs";
    open(filename, O_RDWR | O_CREAT | O_TRUNC);

    CTEST_ASSERT(image_open(filename, 1) == 1, "with truncate");

    remove(filename);
}

void test_image_close() {
    // Setup test file system
    char *filename = "test.vvsfs";
    image_open(filename, 1);

    CTEST_ASSERT(image_close() == 0, "");

    remove(filename);
}

void test_bwrite() {
    // Setup test file system
    char *filename = "test.vvsfs";
    image_open(filename, 1);

    // Set up test environment
    unsigned char test_value = 'B';
    unsigned char write_buffer[BLOCK_SIZE] = {0};
    write_buffer[420] = test_value;

    bwrite(2, write_buffer);

    unsigned char read_buffer[BLOCK_SIZE];
    bread(2, read_buffer);

    CTEST_ASSERT(memcmp(write_buffer, read_buffer, BLOCK_SIZE) == 0, "");

    image_close();
}

void test_bread() {
    // Setup test file system
    char *filename = "test.vvsfs";
    image_open(filename, 1);

    // Set up test environment
    unsigned char test_value = 'B';
    int test_index_num = 420;

    unsigned char reference_buffer[BLOCK_SIZE] = {0};
    reference_buffer[test_index_num] = test_value;

    bwrite(2, reference_buffer);

    unsigned char return_buffer[BLOCK_SIZE];
    bread(2, reference_buffer);

    CTEST_ASSERT(memcmp(return_buffer, reference_buffer, BLOCK_SIZE) == 0, "");

    image_close();
}

void test_find_free() {
    unsigned char test_block[BLOCK_SIZE] = {0};

    set_free(test_block, 0, 1);
    set_free(test_block, 1, 1);

    CTEST_ASSERT(find_free(test_block) == 2, "");
}

void test_set_free() {
    unsigned char test_block[BLOCK_SIZE] = {0};
    int test_bit_num = 0;
    int test_set = 1;

    set_free(test_block, test_bit_num, test_set);

    CTEST_ASSERT(find_free(test_block) == 1, "");
}

void test_ialloc() {
    char *filename = "test.vvsfs";
    image_open(filename, 1);

    ialloc();
    ialloc();
    unsigned char test_inode_map_block[BLOCK_SIZE];

    bread(BLOCK_INODE_MAP, test_inode_map_block);

    CTEST_ASSERT(find_free(test_inode_map_block) == 2, "");

    image_close();
    remove(filename);
}

void test_alloc() {
    char *filename = "test.vvsfs";
    image_open(filename, 1);

    // Alloc 3 times
    alloc();
    alloc();
    alloc();

    unsigned char test_block_map_block[BLOCK_SIZE];

    bread(BLOCK_BLOCK_MAP, test_block_map_block);

    CTEST_ASSERT(find_free(test_block_map_block) == 3, "");

    image_close();
    remove(filename);
}

void test_mkfs() {
    char *filename = "test.vvsfs";
    image_open(filename, 1);
    mkfs();

    unsigned char test_block[BLOCK_SIZE] = {0};
    bread(BLOCK_BLOCK_MAP, test_block);

    CTEST_ASSERT(find_free(test_block) == 7, "");

    image_close();
    remove(filename);
}

void test_find_incore_free() {
    char *filename = "test.vvsfs";
    image_open(filename, 1);
    mkfs();

    // Allocate 2 inodes
    ialloc();
    ialloc();

    CTEST_ASSERT(find_incore_free() == &incore[2],"");

    image_close();
    remove(filename);
}

void test_find_incore_free_no_free_incore() {
    char *filename = "test.vvsfs";
    image_open(filename, 1);
    mkfs();

    // Allocate all inodes
    for (int i = 0; i < MAX_SYS_OPEN_FILES; ++i) {
        ialloc();
    }
}

int main(void) {
    CTEST_VERBOSE(0);

    test_image_open_creates_file();
    test_image_open_without_truncate();
    test_image_open_with_truncate();

    test_image_close();

    test_bwrite();
    test_bread();

    test_set_free();
    test_find_free();

    test_ialloc();
    test_alloc();

    test_mkfs();

    test_find_incore_free();
    test_find_incore_free_no_free_incore()

    CTEST_EXIT();
}
