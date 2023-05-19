#include "ctest.h"
#include "image.h"
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include "block.h"
#include "free.h"
#include "inode.h"
#include "mkfs.h"

char *filename = "test.vvsfs";

void setup_test_enviroment() {
    image_open(filename, 1);
    mkfs();
}

void teardown_test_enviroment() {
    image_close();
    remove(filename);
}

void test_image_open_creates_file() {

    CTEST_ASSERT(image_open(filename, 0) == 0, "file doesn't exist");

    remove(filename);
}

void test_image_open_without_truncate() {
    open(filename, O_RDWR | O_CREAT);

    CTEST_ASSERT(image_open(filename, 0) == 0, "without truncate");

    remove(filename);
}

void test_image_open_with_truncate() {
    open(filename, O_RDWR | O_CREAT | O_TRUNC);

    CTEST_ASSERT(image_open(filename, 1) == 1, "with truncate");

    remove(filename);
}

void test_image_close() {
    setup_test_enviroment();

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
    //TODO implement iget()
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
    setup_test_enviroment();

    incore[0].link_count = 1;
    incore[1].link_count = 1;
    incore[2].link_count = 0;

    struct inode *expected_ptr = &incore[2];

    struct inode *result_ptr = find_incore_free();

    CTEST_ASSERT(result_ptr == expected_ptr,"");

    teardown_test_enviroment();
}

void test_find_incore_free_no_free_incore() {
    setup_test_enviroment();

    // Allocate all inodes
    for (int i = 0; i < MAX_SYS_OPEN_FILES; ++i) {
        incore[i].link_count = 1;
    }

    struct inode *result = find_incore_free();
    CTEST_ASSERT(result == NULL,"");

    teardown_test_enviroment();
}

void test_read_inode_passing() {
    setup_test_enviroment();

    unsigned char *block = {0};
    bread(BLOCK_INODE_DATA_BLOCK_0, block);
    struct inode *test_inode;

    ((struct inode *)block)->inode_num = 420;
    ((struct inode *)block)->owner_id = 2;


    read_inode(test_inode, 420);
    CTEST_ASSERT(test_inode->owner_id == 2,"");

    teardown_test_enviroment();
}

void test_read_inode() {
    test_read_node_passing();
}

void test_find_incore() {

}

void test_find_incore_fail() {

}

void test_iget() {

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
    test_find_incore_free_no_free_incore();

    test_find_incore();
    test_find_incore_fail();

    test_read_inode();
    test_iget();

    CTEST_EXIT();

    teardown_test_enviroment();
}
