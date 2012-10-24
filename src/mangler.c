#include "mangler.h"

#include <string.h>

static int str_starts_with(const char *str, const char *prefix) {
	return strncmp(str, prefix, strlen(prefix)) == 0;
}

int mangler_is_test(const char *name) {
	return str_starts_with(name, "__HtEsT");
}
