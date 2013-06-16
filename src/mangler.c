#include "mangler.h"
#include "herc/herc.h"

#include <string.h>
#include <stdio.h>

static int str_starts_with(const char *str, const char *prefix) {
	return strncmp(str, prefix, strlen(prefix)) == 0;
}

static const char *symbol_type_to_prefix(enum symbol_type st) {
	switch (st) {
	case SYMBOL_IS_TEST : return "__HtEsT_";
	case SYMBOL_IS_BEFORE_TEST: return "__HbEfOrTeSt_";
	case SYMBOL_IS_AFTER_TEST: return "__HaFtErTeSt_";
	case SYMBOL_IS_BEFORE_SUITE: return "__HbEfOrSuItE_";
	case SYMBOL_IS_AFTER_SUITE: return "__HaFtErSuItE_";
	default:
		printf("Unknown symbol type. If you see this message that's a serious bug in Hercules. \
					Please contact the author and tell him that his product sucks!\n");
		exit(1);
	}
}

enum symbol_type mangler_get_symbol_type(const char *name) {
#define TEST_FOR_TYPE(type) if (str_starts_with(name, symbol_type_to_prefix(type))) return type;

	TEST_FOR_TYPE(SYMBOL_IS_TEST);
	TEST_FOR_TYPE(SYMBOL_IS_BEFORE_TEST);
	TEST_FOR_TYPE(SYMBOL_IS_AFTER_TEST);
	TEST_FOR_TYPE(SYMBOL_IS_BEFORE_SUITE);
	TEST_FOR_TYPE(SYMBOL_IS_AFTER_SUITE);
	return SYMBOL_IS_UNKNOWN;

#undef TEST_FOR_TYPE
}

char *mangler_extract_suite(const char *symname, enum symbol_type type) {
	const char *prefix = symbol_type_to_prefix(type);
	return strndup(symname + strlen(prefix), strstr(symname, _HERC_CASE_NAME_DELIMITER_STR) - symname - strlen(prefix));
}

char *mangler_extract_test(const char *symname, enum symbol_type type) {
	return strdup(strstr(symname, _HERC_CASE_NAME_DELIMITER_STR) + strlen(_HERC_CASE_NAME_DELIMITER_STR));
}
