#include "test_desc.h"

#include <stdlib.h>
#include <string.h>

test_desc *test_desc_create(const char *symname) {
	test_desc *test = calloc(1, sizeof(test_desc));
	test->symname = strdup(symname);
	return test;
}

void test_desc_free(test_desc *test) {
	free(test->symname);
	free(test);
}
