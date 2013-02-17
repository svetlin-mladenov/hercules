#include "herc_string_utils.h"

#include <string.h>

const char* string_drop_prefix(const char *prefix, const char *str) {
	size_t n = strlen(prefix);
	
	if (strncmp(prefix, str, n) == 0) {
		return str+n;
	} else {
		return NULL;
	}
}
