#include "ctest.h"

int main(void) {
	CTEST_VERBOSE(on)
	printf("im testing\n ");

	CTEST_ASSERT(image_open("filename",0) == 0);

	return CTEST_EXIT();
}
