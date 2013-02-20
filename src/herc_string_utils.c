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

bool string_array_member(const char **array, size_t n, const char *elem) {
	size_t i;
	for (i = 0; i<n; i++) {
		if (strcmp(array[i], elem) == 0) {
			return true;
		}
	}

	return false;
}
