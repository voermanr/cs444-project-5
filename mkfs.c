#include "image.h"
#include <unistd.h>
#include "block.h"

void mkfs(void) {
    write(image_fd, 0, 1024 * BLOCK_SIZE);
    for (int i = 0; i < 7; ++i) {
        alloc();
    }
}