simfs: image.o
	gcc -Wall -Wextra -o $@ $^


image.o:
	gcc -Wall -Wextra -c image.c

simfs_test: simfs_test.c image.o
	gcc -Wall -Wextra -o $@ $^ -DCTEST_ENABLE
	./simfs_test
