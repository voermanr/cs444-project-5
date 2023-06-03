#include <stdlib.h>
#include "inode.h"
#include "directory.h"
#include "pack.h"
#include "mkfs.h"
#include "directory.h"
#include <string.h>
#include <stdio.h>

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
    if (!strcmp(path, "/")) {
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
    if (strcmp(path, "/")) {
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