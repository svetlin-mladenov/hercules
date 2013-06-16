#ifndef SUITE_DESC_H_
#define SUITE_DESC_H_

#include <cryad/list.h>

typedef void* (*before_suite_fn)(void *);
typedef void* (*before_test_fn)(void *);
typedef void (*after_test_fn)(void *);
typedef void (*after_suite_fn)(void *);

typedef struct suite_desc {
	char *name;
	before_suite_fn before_suite;
	before_test_fn before_test;
	after_test_fn after_test;
	after_suite_fn after_suite;
	cr_list *tests;
} suite_desc;

suite_desc *suite_desc_new(const char *name);

void suite_desc_free(suite_desc *);

int suite_desc_cmp(const suite_desc *s1, const suite_desc *s2);

unsigned suite_desc_hash(const suite_desc *suite);


#endif
