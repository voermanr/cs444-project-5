#include "ctest.h"
#include "image.h"
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include "block.h"
#include "free.h"
#include "inode.h"
#include "mkfs.h"

// Uber setup
char *filename = "test.vvsfs";

void setup_test_enviroment() {
    image_open(filename, 1);
    mkfs();
}

void _and_allocate_all_nodes() {
    for (int i = 0; i < MAX_SYS_OPEN_FILES; ++i) {
        incore[i].ref_count = 1;
    }
}

void _and_teardown_test_enviroment() {
    image_close();
    remove(filename);
}

// individual tests functions are kind of like:
//
// Given
//  setup_test_enviroment();_and_allocate_all_nodes();
//
// When
//  struct inode *result = find_incore_free();
//
// Then
//  CTEST_ASSERT(result == NULL,"");_and_tear_down_test_environment();

void test_iget() {

}


// IMAGE OPEN()
void test_image_open_creates_file() {

    CTEST_ASSERT(image_open(filename, 0) == 0, "file doesn't exist");

    remove(filename);
}

void test_image_open_pass() {
    open(filename, O_RDWR | O_CREAT);

    CTEST_ASSERT(image_open(filename, 0) == 0, "without truncate");

    remove(filename);
}

void test_image_open_truncate_pass() {
    open(filename, O_RDWR | O_CREAT | O_TRUNC);

    CTEST_ASSERT(image_open(filename, 1) == 1, "with truncate");

    remove(filename);
}

void test_image_open() {
    test_image_open_creates_file();
    test_image_open_pass();
    test_image_open_truncate_pass();
}


// IMAGE_CLOSE()
void test_image_close_pass() {
    setup_test_enviroment();

    CTEST_ASSERT(image_close() == 0, "");

    remove(filename);
}

void test_image_close_fail() {

}

void test_image_close() {
    test_image_close_pass();
    //TODO test_image_close_fail();
}


// BWRITE()
void test_bwrite_pass() {
    setup_test_enviroment();
    unsigned char test_value = 'B';
    unsigned char write_buffer[BLOCK_SIZE] = {0};
    unsigned char read_buffer[BLOCK_SIZE];
    write_buffer[420] = test_value;


    bwrite(2, write_buffer);
    //TODO remove dependency on bread (man, in real life too)
    bread(2, read_buffer);


    CTEST_ASSERT(memcmp(write_buffer, read_buffer, BLOCK_SIZE) == 0, "");
    _and_teardown_test_enviroment();
}

void test_bwrite_fail() {

}

void test_bwrite() {
    test_bwrite_pass();
    //TODO test_bwrite_fail()
}


// BREAD()
void test_bread_pass() {
    setup_test_enviroment();
    unsigned char test_value = 'B';
    int test_index_num = 420;
    unsigned char reference_buffer[BLOCK_SIZE] = {0};
    unsigned char return_buffer[BLOCK_SIZE];


    //TODO remove dependency on bwrite
    reference_buffer[test_index_num] = test_value;
    bwrite(2, reference_buffer);
    bread(2, reference_buffer);


    CTEST_ASSERT(memcmp(return_buffer, reference_buffer, BLOCK_SIZE) == 0, "");
    _and_teardown_test_enviroment();
}

void test_bread_fail() {

}

void test_bread() {
    test_bread_pass();
    //TODO test_bread_fail();

}


// SET_FREE()
void test_set_free_pass() {
    unsigned char test_block[BLOCK_SIZE] = {0};
    int test_bit_num = 0;
    int test_set = 1;

    set_free(test_block, test_bit_num, test_set);

    CTEST_ASSERT(find_free(test_block) == 1, "");
}

void test_set_free_fail() {

}

void test_set_free() {
    test_set_free_pass();
    //TODO test_set_free_fail();
}


// FIND_FREE()
void test_find_free_pass() {
    unsigned char test_block[BLOCK_SIZE] = {0};

    set_free(test_block, 0, 1);
    set_free(test_block, 1, 1);

    CTEST_ASSERT(find_free(test_block) == 2, "");
}

void test_find_free_fail() {

}

void test_find_free() {
    test_find_free_pass();
    //TODO test_find_free_fail();
}


// IALLOC()
void test_ialloc_pass() {
    setup_test_enviroment();
    //TODO I think this is gonna break
    unsigned char test_inode_map_block[BLOCK_SIZE];

    ialloc();
    ialloc();
    //TODO remove dependency on bread
    bread(BLOCK_INODE_MAP, test_inode_map_block);

    CTEST_ASSERT(find_free(test_inode_map_block) == 2, "");
    _and_teardown_test_enviroment();
}

void test_ialloc_fail() {

}

void test_ialloc() {
    test_ialloc_pass();
    //TODO test_ialloc_fail();
}


// ALLOC()
void test_alloc_pass() {//TODO implement iget()
    setup_test_enviroment();
    unsigned char test_block_map_block[BLOCK_SIZE];

    // Alloc 3 times
    alloc();
    alloc();
    alloc();
    //TODO remove dependency on bread
    bread(BLOCK_BLOCK_MAP, test_block_map_block);

    CTEST_ASSERT(find_free(test_block_map_block) == 3, "");
    _and_teardown_test_enviroment();
}

void test_alloc_fail() {

}

void test_alloc() {
    //TODO test_alloc_pass();
    //TODO test_alloc_fail();
}


// MKFS()
void test_mkfs_pass() {
    image_open(filename, 1);
    unsigned char test_block[BLOCK_SIZE] = {0};

    mkfs();
    bread(BLOCK_BLOCK_MAP, test_block);

    CTEST_ASSERT(find_free(test_block) == 7, "");
    _and_teardown_test_enviroment();
}

void test_mkfs_fail() {

}

void test_mkfs() {
    test_mkfs_pass();
    //TODO test_mkfs_fail();
}


// FIND_INCORE_FREE()
void test_find_incore_free_pass() {
    setup_test_enviroment();
    incore[0].ref_count = 1;
    incore[1].ref_count = 1;
    incore[2].ref_count = 0;

    struct inode *expected_ptr = &incore[2];
    struct inode *result_ptr = find_incore_free();

    CTEST_ASSERT(result_ptr == expected_ptr,"");_and_teardown_test_enviroment();
}

void test_find_incore_free_fail() {
    setup_test_enviroment();_and_allocate_all_nodes();

    struct inode *result = find_incore_free();

    CTEST_ASSERT(result == NULL,"");_and_teardown_test_enviroment();
}

void test_find_incore_free() {
    test_find_incore_free_pass();
    test_find_incore_free_fail();
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

    test_image_open();

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

    _and_teardown_test_enviroment();
    CTEST_EXIT();

    teardown_test_enviroment();
}
