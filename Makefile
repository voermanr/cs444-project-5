.PHONY: clean
.PHONY: all
.PHONY: test

simfs: image.o block.o
	gcc -Wall -Wextra -o $@ $^

%.o: %.c %.h
	gcc -Wall -Wextra -c -o $@ $<

test: simfs_test.c block.o free.o image.o inode.o mkfs.o
	gcc -Wall -Wextra -g -o $@ $^ -DCTEST_ENABLE

clean:

all:
	gcc -Wall -Wextra -c $@ $^