#include <stdlib.h>
#include "inode.h"
#include "directory.h"
#include "pack.h"
#include "mkfs.h"
#include "directory.h"
#include <string.h>
#include <stdio.h>

#define ROOT_PATH "/"

// Some lifted helper functions from Beej
char *get_dirname(const char *path, char *dirname)
{
    strcpy(dirname, path);

    char *p = strrchr(dirname, '/');

    if (p == NULL) {
        strcpy(dirname, ".");
        return dirname;
    }

    if (p == dirname)  // Last slash is the root /
        *(p+1) = '\0';

    else
        *p = '\0';  // Last slash is not the root /

    return dirname;
}

char *get_basename(const char *path, char *basename)
{
    if (strcmp(path, ROOT_PATH) == 0) {
        strcpy(basename, path);
        return basename;
    }

    const char *p = strrchr(path, '/');

    if (p == NULL)
        p = path;   // No slash in name, start at beginning
    else
        p++;        // Start just after slash

    strcpy(basename, p);

    return basename;
}



struct directory *directory_open(int inode_num) {
    struct inode *directory_inode = iget(inode_num);
    //printf("directory_open(): directory_inode.size: %d\n", directory_inode->size);
    if (directory_inode == NULL || directory_inode->flags != DIRECTORY) {
        return NULL;
    }

    //printf("directory_open(): \tdirectory_inode: %p, \tsize: %d\n", directory_inode, directory_inode->size);

    struct directory *dir = malloc(sizeof(struct directory)); //TODO free this
    dir->inode = directory_inode;
    dir->offset = 0;
    return dir;
}

int directory_get(struct directory *dir, struct directory_entry *ent) {
    unsigned char block[BLOCK_SIZE] = {0};
    strcpy(ent->name,"");
    ent->inode_num = -1;
    unsigned int offset = dir->offset;
    unsigned int size = dir->inode->size;
    //printf("directory_get(): \tdir->offset: %d, \tdir->inode->size: %d\n", offset, size);

    if (offset >= size) {
        //printf("no more entries\n");
        return -1;
    }

    unsigned int data_block_index = offset / BLOCK_SIZE;
    unsigned int data_block_num = dir->inode->block_ptr[data_block_index];
    //printf("data_block_num: %d\n", data_block_num);
    bread(data_block_num, block);

    unsigned char *offset_in_block = block + (offset % BLOCK_SIZE);
    ent->inode_num = read_u16(offset_in_block);
    strcpy(ent->name, (char*)(offset_in_block + 2));

    dir->offset = offset + DIRECTORY_ENTRY_SIZE_ON_DISK;

    return 0;
}

void directory_close(struct directory *d) {
    iput(d->inode);
    free(d);
}

struct inode *namei(char *path) {
    if (!strcmp(path, ROOT_PATH)) {
        return iget(ROOT_INODE_NUM);
    }
    else { /* TODO: implement:
              *  /foo,
              *  /foo/bar
              *  functionality.*/
    }

    return NULL;
}

int directory_make(char *path) {
    char base_name[strlen(path)];
    get_basename(path, base_name);

    //TODO: assuming an absolute path for now
    if (path[0] == '/' && strcmp(base_name, ROOT_PATH)) {
        //TODO: assuming directories can only be made from the root directory
        struct inode *parent_directory_inode = namei("/");
        struct inode *new_directory_inode = ialloc();
        unsigned char new_directory_data_block[BLOCK_SIZE] = { 0 };
        unsigned int new_directory_data_block_num = alloc();
        bread(new_directory_data_block_num, new_directory_data_block);
        unsigned char *offset_ptr = new_directory_data_block;
        write_u16(offset_ptr, new_directory_inode->inode_num);
        offset_ptr += 2;
        strcpy((char *)(offset_ptr), ".");
        offset_ptr +=16;
        offset_ptr +=14;//Padding
        write_u16(offset_ptr, parent_directory_inode->inode_num);
        offset_ptr += 2;
        strcpy((char *)(offset_ptr), "..");

        new_directory_inode->ref_count = 1;
        new_directory_inode->flags = DIRECTORY;
        new_directory_inode->size = DIRECTORY_ENTRY_SIZE_ON_DISK * DIRECTORY_DEFAULT_ENTRIES;
        new_directory_inode->block_ptr[0] = new_directory_data_block_num;

        bwrite(new_directory_data_block_num, new_directory_data_block);

        unsigned int parent_directory_data_block_num = parent_directory_inode->block_ptr[0];
        unsigned char parent_directory_data_block[BLOCK_SIZE] = {0};
        unsigned int parent_directory_data_block_offset_num = parent_directory_inode->size / BLOCK_SIZE;
        parent_directory_data_block_num += parent_directory_data_block_offset_num;

        bread(parent_directory_data_block_num, parent_directory_data_block);
        unsigned char *parent_directory_block_offset_ptr = parent_directory_data_block + parent_directory_inode->size;
        write_u16(parent_directory_block_offset_ptr, new_directory_inode->inode_num);
        parent_directory_block_offset_ptr += 2;
        strcpy((char *)(parent_directory_block_offset_ptr), base_name);

        bwrite(parent_directory_data_block_num, parent_directory_data_block);

        parent_directory_inode->size += DIRECTORY_ENTRY_SIZE_ON_DISK;

        iput(new_directory_inode);
        iput(parent_directory_inode);
        return 0;
    }

    return -1;
}


/* void main(void) {
    mkfs();
    struct directory *dir;
    dir = directory_open(0);
    struct directory_entry ent;
    directory_get(dir, &ent);
} */