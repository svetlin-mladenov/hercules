#ifndef MANGLER_H
#define MANGLER_H

int mangler_is_test(const char *);

char *mangler_extract_suite(const char *);

char *mangler_extract_test(const char *);

#endif
