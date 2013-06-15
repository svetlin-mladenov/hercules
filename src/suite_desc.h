#ifndef SUITE_DESC_H_
#define SUITE_DESC_H_

#include <cryad/list.h>

typedef struct suite_desc {
	char *name;
	cr_list *tests;
} suite_desc;

suite_desc *suite_desc_new(const char *name);

void suite_desc_free(suite_desc *);

int suite_desc_cmp(const suite_desc *s1, const suite_desc *s2);

unsigned suite_desc_hash(const suite_desc *suite);


#endif
