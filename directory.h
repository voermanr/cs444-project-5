#ifndef DIRECTORY_H
#define DIRECTORY_H

#define ROOT_INODE_NUM 0
#define DIRECTORY_ENTRY_SIZE_ON_DISK 32

#define DIRECTORY_DEFAULT_ENTRIES 2

struct directory {
    struct inode *inode;
    unsigned int offset;
};

struct directory_entry {
    unsigned int inode_num;
    char name[16];
};

struct directory *directory_open(int inode_num);
int directory_get(struct directory *dir, struct directory_entry *ent);
void directory_close(struct directory *d);

struct inode *namei(char *path);
int directory_make(char *path);
#endif