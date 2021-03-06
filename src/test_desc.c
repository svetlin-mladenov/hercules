#include "test_desc.h"

#include <stdlib.h>
#include <string.h>

#include "mangler.h"

test_desc *test_desc_create(const char *symname) {
	test_desc *test = calloc(1, sizeof(test_desc));
	test->symname = strdup(symname);
	test->suit = mangler_extract_suite(symname, SYMBOL_IS_TEST);
	test->test = mangler_extract_test(symname, SYMBOL_IS_TEST);
	return test;
}

void test_desc_free(test_desc *test) {
	free(test->suit);
	free(test->test);
	free(test->symname);
	free(test);
}
