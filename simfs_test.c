#include "ctest.h"
#include "image.h"
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include "block.h"
#include "free.h"
#include "inode.h"
#include "mkfs.h"
#include "directory.h"
#include "ls.c"


// Uber setup
char *filename = "test.vvsfs";

struct inode *a_non_null_inode() {
    struct inode pointable_inode;
    struct inode *result_address = &pointable_inode;
    return result_address;
}

void setup_test_enviroment() {
    image_open(filename, 1);
    clear_incore_inodes();
    mkfs();
}

void and_allocate_all_nodes() {
    for (int i = 0; i < MAX_SYS_OPEN_FILES; ++i) {
        set_incore_inode(i, a_non_null_inode());
    }
}

void and_fill_bitmap(unsigned int block_num) {
    unsigned char block[BLOCK_SIZE] = {0};
    bread(block_num, block);
    memset(block, 0xFF, sizeof(block));
    bwrite(block_num,block);
}

void and_finally_teardown_test_environment() {
    image_close();
    clear_incore_inodes();
    remove(filename);
}

// individual tests functions are kind of like:
//
// Given
//  setup_test_enviroment();and_allocate_all_nodes();
//
// When
//  struct inode *result = find_incore_free();
//
// Then
//  CTEST_ASSERT(result == NULL,"");_and_tear_down_test_environment();


// IMAGE OPEN()
void test_image_open_creates_file() {
    CTEST_ASSERT(image_open(filename, 0) == 0, "file doesn't exist");
    and_finally_teardown_test_environment();
}

void test_image_open_pass() {
    open(filename, O_RDWR | O_CREAT, 0600);

    CTEST_ASSERT(image_open(filename, 0) == 0, "without truncate");
    and_finally_teardown_test_environment();
}

void test_image_open_truncate_pass() {
    open(filename, O_RDWR | O_CREAT | O_TRUNC, 0600);

    CTEST_ASSERT(image_open(filename, 1) == 1, "with truncate");
    and_finally_teardown_test_environment();
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
    and_finally_teardown_test_environment();
}

void test_image_close() {
    test_image_close_pass();
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
    and_finally_teardown_test_environment();
}

void test_bwrite() {
    test_bwrite_pass();
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
    and_finally_teardown_test_environment();
}

void test_bread() {
    test_bread_pass();
}


// SET_FREE()
void test_set_free_pass() {
    unsigned char test_block[BLOCK_SIZE] = {0};
    int test_bit_num = 0;
    int test_set = 1;

    set_free(test_block, test_bit_num, test_set);

    CTEST_ASSERT(find_free(test_block) == 1, "");
}

void test_set_free() {
    test_set_free_pass();
}


// FIND_FREE()
void test_find_free_pass() {
    unsigned char test_block[BLOCK_SIZE] = {0};

    set_free(test_block, 0, 1);
    set_free(test_block, 1, 1);

    CTEST_ASSERT(find_free(test_block) == 2, "");
}

void test_find_free_fail() {
    setup_test_enviroment();
    and_fill_bitmap(BLOCK_INODE_MAP);
    unsigned char the_inode_map_block[BLOCK_SIZE];
    bread(BLOCK_INODE_MAP, the_inode_map_block);

    CTEST_ASSERT(find_free(the_inode_map_block) == -1,"");
    and_finally_teardown_test_environment();
}

void test_find_free() {
    test_find_free_pass();
    test_find_free_fail();
}


int max(int a, int b) {
    return (a > b) ? a : b;
}

// IALLOC()
void test_ialloc_pass() {
    setup_test_enviroment();
    struct inode *test_inode = NULL;

    test_inode = ialloc();
    CTEST_ASSERT(test_inode->inode_num != 0 &&
                 test_inode->size == 0 &&
                 test_inode->ref_count == 1 &&
                 test_inode->owner_id == 0 &&
                 test_inode->flags == 0,
                 "default inode has been set");

    and_finally_teardown_test_environment();
}

void test_ialloc_fail() {
    //I think ialloc just depends on other functions for its failure.
}

void test_ialloc_project_6() {
    test_ialloc_pass();
    test_ialloc_fail(); //empty
}


// ALLOC()
void test_alloc_pass() {
    setup_test_enviroment();
    unsigned char block[BLOCK_SIZE] = {0};

    // Alloc 3 times
    alloc();
    alloc();
    alloc();
    //TODO remove dependency on bread
    bread(BLOCK_BLOCK_MAP, block);

    CTEST_ASSERT(alloc() == 11, "");
    and_finally_teardown_test_environment();
}

void test_alloc_fail() {
    setup_test_enviroment();
    and_fill_bitmap(BLOCK_BLOCK_MAP);

    CTEST_ASSERT(alloc() == -1, "");
    and_finally_teardown_test_environment();
}

void test_alloc() {
    test_alloc_pass();
    test_alloc_fail();
}


// MKFS()
void test_mkfs_pass() {
    clear_incore_inodes();
    image_open(filename, 1);
    unsigned char test_block[BLOCK_SIZE] = {0};

    mkfs();
    bread(BLOCK_BLOCK_MAP, test_block);

    CTEST_ASSERT(find_free(test_block) == 8, "");
    and_finally_teardown_test_environment();
}

void test_mkfs_fail() {
    //how?
}

void test_mkfs() {
    test_mkfs_pass();
    test_mkfs_fail();
}


// FIND_INCORE_FREE()
void test_find_incore_free_pass() {
    setup_test_enviroment();
    set_incore_inode(0, a_non_null_inode());
    set_incore_inode(1, a_non_null_inode());
    unset_incore_inode(2);
    struct inode *expected_ptr = get_incore_inode_address(2);

    struct inode *result_ptr = find_incore_free();

    CTEST_ASSERT(result_ptr == expected_ptr,"");
    and_finally_teardown_test_environment();
}

void test_find_incore_free_fail() {
    setup_test_enviroment();
    and_allocate_all_nodes();

    struct inode *result = find_incore_free();

    CTEST_ASSERT(result == NULL,"");
    and_finally_teardown_test_environment();
}

void test_find_incore_free() {
    test_find_incore_free_pass();
    test_find_incore_free_fail();
}


// FIND_INCORE()
void test_find_incore_pass() {
    setup_test_enviroment();
    int pos = 12;
    unsigned int inode_num = 13;
    unsigned int size = 69;

    struct inode test_inode = {0};
    test_inode.ref_count = 1;
    test_inode.inode_num = inode_num;
    test_inode.size = size;
    set_incore_inode(pos, &test_inode);

    struct inode *found_inode = find_incore(inode_num);

    CTEST_ASSERT(found_inode->size == size,"");
    and_finally_teardown_test_environment();
}

void test_find_incore_fail() {
    setup_test_enviroment();
    int nonexistent_inode_number = 22;
    set_incore_inode_and_inode_num(0, 21);
    set_incore_inode_and_inode_num(1, 23);

    struct inode *result_address = a_non_null_inode();

    result_address = find_incore(nonexistent_inode_number);

    CTEST_ASSERT(result_address == NULL,"");
    and_finally_teardown_test_environment();
}

void test_find_incore() {
    test_find_incore_pass();
    test_find_incore_fail();
}


// WRITE_INODE()
void test_write_inode_pass() {
    setup_test_enviroment();

    int inode_num = 69;
    struct inode setup_inode, test_inode;
    setup_inode.inode_num = inode_num;
    setup_inode.size = 100;

    write_inode(&setup_inode);
    read_inode(&test_inode, inode_num);

    CTEST_ASSERT(test_inode.size == 100, "");
    and_finally_teardown_test_environment();
}

void test_write_inode() {
    test_write_inode_pass();
}


// READ_INODE()
void test_read_inode_pass() {
    setup_test_enviroment();

    int inode_num = 3;
    int owner_id = 69;
    struct inode setup_inode, test_inode;
    setup_inode.inode_num = inode_num;
    setup_inode.owner_id = owner_id;

    write_inode(&setup_inode);
    read_inode(&test_inode, inode_num);

    CTEST_ASSERT(test_inode.owner_id == owner_id, "");
    and_finally_teardown_test_environment();
}

void test_read_inode() {
    test_read_inode_pass();
}


// IGET()
void test_iget_pass_already_exists() {
    setup_test_enviroment();
    unsigned int inode_num = 69;
    struct inode writable_inode = {0};
    writable_inode.inode_num = inode_num;
    write_inode(&writable_inode);

    struct inode *result_address = 0;

    result_address = iget(inode_num);

    CTEST_ASSERT(result_address->inode_num == 69,"");
    and_finally_teardown_test_environment();
}

void test_iget_pass_create_inode() {
    setup_test_enviroment();
    unsigned int inode_num = 69;
    struct inode *result_address = a_non_null_inode();
    struct inode *expected_address = 0;

    result_address = iget(inode_num);
    expected_address = find_incore(inode_num);

    CTEST_ASSERT(result_address == expected_address,"");
    and_finally_teardown_test_environment();
}

void test_iget_fail() {
    setup_test_enviroment();
    and_allocate_all_nodes();
    and_fill_bitmap(BLOCK_INODE_MAP);
    unsigned int nonexistent_inode_num = 67;

    struct inode *iget_result = iget(nonexistent_inode_num);

    CTEST_ASSERT(iget_result == NULL,"");
    and_finally_teardown_test_environment();
}

void test_iget() {
    test_iget_pass_already_exists();
    test_iget_pass_create_inode();
    test_iget_fail();
}


// IPUT()
void test_iput_pass_decrement(){
    setup_test_enviroment();
    unsigned int some_arbitrary_ref_count = 22;

    struct inode an_inode = {0};
    an_inode.ref_count = some_arbitrary_ref_count;

    iput(&an_inode);

    CTEST_ASSERT(an_inode.ref_count == some_arbitrary_ref_count - 1,"");
    and_finally_teardown_test_environment();
}

void test_iput_pass_no_more_ref() {
    setup_test_enviroment();
    unsigned int ref_count = 1;
    unsigned int inode_num = 69;

    struct inode an_inode = {0};
    an_inode.ref_count = ref_count;
    an_inode.inode_num = inode_num;
    struct inode a_different_inode = {0};

    iput(&an_inode);
    read_inode(&a_different_inode, inode_num);

    CTEST_ASSERT(a_different_inode.inode_num == inode_num,"");
    and_finally_teardown_test_environment();
}

void test_iput(){
    test_iput_pass_decrement();
    test_iput_pass_no_more_ref();
}

void tests_project_5() {
    test_image_open();

    test_image_close();

    test_bwrite();

    test_bread();

    test_set_free();

    test_find_free();

    test_alloc();

    test_mkfs();
}

void tests_project_6() {
    test_find_incore_free();

    test_find_incore();

    test_write_inode();

    test_read_inode();

    test_iget();

    test_iput();

    test_ialloc_project_6();
}


void test_mkfs_ialloc_root_inode() {
    setup_test_enviroment();
    unsigned char inode_map_block[BLOCK_SIZE] = {0};

    bread(BLOCK_INODE_MAP, inode_map_block);

    CTEST_ASSERT(find_free(inode_map_block) == 1, "");
    and_finally_teardown_test_environment();
}

void test_mkfs_alloc_root_data_block() {
    setup_test_enviroment();
    unsigned char block_map_block[BLOCK_SIZE] = {0};

    bread(BLOCK_BLOCK_MAP, block_map_block);

    CTEST_ASSERT(find_free(block_map_block) == 8, "");
    and_finally_teardown_test_environment();
}

void test_mkfs_inode_root_initialized() {
    setup_test_enviroment();
    struct directory *dir;
    struct directory_entry ent;
    dir = directory_open(ROOT_INODE_NUM);
    //printf("directory_get(): \tdir->offset: %d, \tdir->inode->size: %d\n", dir->offset, dir->inode->size);

    directory_get(dir, &ent);
    unsigned int inode_num = ent.inode_num;

    struct inode in;
    read_inode(&in, inode_num);
    //printf("flags: %d, size: %d\n", in.flags, in.size);
    CTEST_ASSERT(in.flags == 2, "");
    CTEST_ASSERT(in.size == 64, "");
    CTEST_ASSERT(in.block_ptr[0], "");
    and_finally_teardown_test_environment();
}

void test_mkfs_7() {
    test_mkfs_ialloc_root_inode();
    test_mkfs_alloc_root_data_block();
    test_mkfs_inode_root_initialized();
}


// Directory
void test_directory_open_pass() {
    setup_test_enviroment();

    CTEST_ASSERT(directory_open(0),"");
    and_finally_teardown_test_environment();
}

void test_directory_get_pass() {
    setup_test_enviroment();
    struct directory *dir;
    struct directory_entry ent;
    dir = directory_open(ROOT_INODE_NUM);

    CTEST_ASSERT(directory_get(dir, &ent) == 0, "");
    CTEST_ASSERT(ent.inode_num == ROOT_INODE_NUM, "");
    CTEST_ASSERT(!strcmp(ent.name,"."), "");

    CTEST_ASSERT(directory_get(dir, &ent) == 0, "");
    CTEST_ASSERT(ent.inode_num == ROOT_INODE_NUM, "");
    CTEST_ASSERT(!strcmp(ent.name,".."),"");
    and_finally_teardown_test_environment();
}

void test_directory_open_fail() {
    setup_test_enviroment();

    struct directory *test_dir = directory_open(2);
    CTEST_ASSERT(test_dir == NULL, "");

    and_finally_teardown_test_environment();
}

void test_directory_get_fail() {
    setup_test_enviroment();
    struct directory *test_root_dir = directory_open(ROOT_INODE_NUM);
    struct directory_entry test_ent;

    directory_get(test_root_dir, &test_ent);
    directory_get(test_root_dir, &test_ent);

    CTEST_ASSERT(directory_get(test_root_dir, &test_ent),"");

    and_finally_teardown_test_environment();
}

void test_directory_close() {
    setup_test_enviroment();
    struct directory *test_directory = directory_open(ROOT_INODE_NUM);

    directory_close(test_directory);
    int directory_closed_happened = 1;

    CTEST_ASSERT(directory_closed_happened,"");
    and_finally_teardown_test_environment();
}

void test_ls(void) {
    setup_test_enviroment();

    //printf("ls:/\n");
    //ls();

    and_finally_teardown_test_environment();
}

void tests_project_7() {
    test_mkfs_7();
    test_directory_open_pass();
    test_directory_open_fail();
    test_directory_get_pass();
    test_directory_get_fail();
    test_directory_close();
    test_ls();
}

void test_namei_pass_root_inode() {
    setup_test_enviroment();
    struct inode* root_inode_ptr = iget(ROOT_INODE_NUM);

    CTEST_ASSERT(namei("/") == root_inode_ptr,"");
    and_finally_teardown_test_environment();
}

void test_namei_fail() {
    setup_test_enviroment();
    char *not_the_root_path = "/clowns";

    CTEST_ASSERT(namei(not_the_root_path) == NULL,"");
    and_finally_teardown_test_environment();
}

void test_directory_make_pass() {
    setup_test_enviroment();
    char *test_path = "/clowns";
    int success = 0;
    struct directory_entry ent;
    struct directory *root_dir = directory_open(ROOT_INODE_NUM);
    unsigned int assumed_new_directory_inode_num = 1;
    unsigned int number_of_created_directories = 2;

    CTEST_ASSERT(directory_make(test_path) == success,"");
    CTEST_ASSERT(directory_get(directory_open(assumed_new_directory_inode_num),&ent) == 0,"");
    CTEST_ASSERT(directory_open(number_of_created_directories) == NULL,"");

    directory_get(root_dir, &ent);
    CTEST_ASSERT(ent.inode_num == ROOT_INODE_NUM,"");
    directory_get(root_dir, &ent);
    CTEST_ASSERT(ent.inode_num == ROOT_INODE_NUM,"");
    directory_get(root_dir, &ent);
    CTEST_ASSERT(ent.inode_num == assumed_new_directory_inode_num,"");
    and_finally_teardown_test_environment();
}

void test_directory_make_fail() {
    setup_test_enviroment();
    char *test_path_but_bad = "erykahbadu";
    int failure = -1;

    CTEST_ASSERT(directory_make(test_path_but_bad) == failure,"");
    and_finally_teardown_test_environment();

}

void tests_project_9() {
    test_namei_pass_root_inode();
    test_namei_fail();

    test_directory_make_pass();
    test_directory_make_fail();
}

int main(void) {
    CTEST_VERBOSE(0);

    tests_project_5();

    tests_project_6();

    tests_project_7();

    tests_project_9();

    and_finally_teardown_test_environment();

    CTEST_RESULTS();
    CTEST_EXIT();
}
