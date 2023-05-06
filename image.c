#include <unistd.h>
#include <fcntl.h>

int image_fd;

int image_open(char *filename, int truncate) {
    if (truncate) {
        image_fd = open(filename, O_RDWR | O_CREAT | O_TRUNC, 0600);
        return 1;
    } else {
        image_fd = open(filename, O_RDWR | O_CREAT, 0600);
        return 0;
    }
}

int image_close(void) {
    close(image_fd);
    return 0;
}
