.PHONY: clean
.PHONY: all
.PHONY: test

simfs: image.o block.o
	gcc -Wall -Wextra -o $@ $^

%.o: %.c %.h
	gcc -Wall -Wextra -c -o $@ $<

test: simfs_test.c block.o free.o image.o inode.o mkfs.o pack.o directory.o
	gcc -Wall -Wextra -g -o $@ $^ -DCTEST_ENABLE

clean:
	@rm *.o test simfs

all:
	gcc -Wall -Wextra -c $@ $^

mkfs_test: mkfs.c block.o inode.o image.o pack.o free.o
	gcc -Wall -Wextra -g -o $@ $^ -DCTEST_ENABLE

inode_test: inode.c block.o free.o pack.o image.o mkfs.o
	gcc -Wall -Wextra -g -o $@ $^ -DCTEST_ENABLE

directory_test: directory.c inode.o pack.o block.o free.o image.o mkfs.o
	gcc -Wall -Wextra -g -o $@ $^ -DCTEST_ENABLE