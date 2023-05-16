.PHONY: clean
.PHONY: all

simfs: image.o block.o
	gcc -Wall -Wextra -o $@ $^

%.o: %.c %.h
	gcc -Wall -Wextra -c -o $@ $<

simfs_test: simfs_test.c block.o free.o image.o inode.o mkfs.o
	gcc -Wall -Wextra -g -o $@ $^ -DCTEST_ENABLE
	./simfs_test
	rm simfs_test

clean:
	rm *.o

all:
	gcc -Wall -Wextra -c $@ $^