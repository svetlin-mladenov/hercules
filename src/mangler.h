#ifndef MANGLER_H
#define MANGLER_H

enum symbol_type {SYMBOL_IS_TEST, SYMBOL_IS_BEFORE_TEST, SYMBOL_IS_AFTER_TEST, SYMBOL_IS_BEFORE_SUITE, SYMBOL_IS_AFTER_SUITE,
					SYMBOL_IS_UNKNOWN};

enum symbol_type mangler_get_symbol_type(const char *);

char *mangler_extract_suite(const char *, enum symbol_type type);

char *mangler_extract_test(const char *, enum symbol_type type);

#endif
