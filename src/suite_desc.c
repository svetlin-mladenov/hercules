#include "suite_desc.h"
#include "test_desc.h"

#include <cryad/slist.h>

#include <string.h>
#include <stddef.h>

suite_desc *suite_desc_new(const char *name) {
	suite_desc *suite = malloc(sizeof(suite_desc));
	suite->name = strdup(name);
	suite->tests = cr_slist_create((void (*)(void*))test_desc_free);
	return suite;
}

void suite_desc_free(suite_desc *suite) {
	if (suite != 0) {
		cr_list_free(suite->tests);
		free(suite->name);
		free(suite);
	}
}

int suite_desc_cmp(const suite_desc *s1, const suite_desc *s2) {
	return strcmp(s1->name, s2->name);
}

unsigned suite_desc_hash(const suite_desc *suite) {
	unsigned hash = 0;
	size_t i = 0;

	if (suite == NULL)
		return hash;

	for (i = 0; suite->name[i]; i++) {
		hash = 31*hash + suite->name[i];
	}

	return hash;
}

