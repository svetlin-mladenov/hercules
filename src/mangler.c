#include "mangler.h"
#include "herc/herc.h"

#include <string.h>

static int str_starts_with(const char *str, const char *prefix) {
	return strncmp(str, prefix, strlen(prefix)) == 0;
}

int mangler_is_test(const char *name) {
	return str_starts_with(name, "__HtEsT_");
}

char *mangler_extract_suite(const char *symname) {
	const char *prefix = "__HtEsT_";
	return strndup(symname + strlen(prefix), strstr(symname, _HERC_CASE_NAME_DELIMITER_STR) - symname - strlen(prefix));
}

char *mangler_extract_test(const char *symname) {
	return strdup(strstr(symname, _HERC_CASE_NAME_DELIMITER_STR) + strlen(_HERC_CASE_NAME_DELIMITER_STR));
}
