#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>

int image_fd;

int image_open(char *filename, int truncate) {
    if (truncate) {
        image_fd = open(filename, O_RDWR | O_CREAT | O_TRUNC, 0644);
        //printf("image_open(): \timage_fd: %d\n", image_fd);
    } else {
        image_fd = open(filename, O_RDWR | O_CREAT, 0644);
    }

    if (image_fd == -1) {
        printf("open error: %s\n", strerror(errno));
        return -1;
    }

    return truncate;
}

int image_close(void) {
    close(image_fd);
    return 0;
}
