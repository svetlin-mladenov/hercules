#ifndef TEST_DESC_H
#define TEST_DESC_H

typedef struct test_desc {
	char *symname;
} test_desc;

test_desc *test_desc_create(const char *);

void test_desc_free(test_desc*);

#endif
