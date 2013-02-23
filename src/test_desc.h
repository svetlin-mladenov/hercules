#ifndef TEST_DESC_H
#define TEST_DESC_H

enum test_status {NOT_RUN, FAILED, PASSED, FILTERED};

typedef struct test_desc {
	char *symname;
	char *suit;
	char *test;
	enum test_status status;
	double ellapsed_time;
	const char *fail_msg;
} test_desc;

test_desc *test_desc_create(const char *);

void test_desc_free(test_desc*);

#endif
