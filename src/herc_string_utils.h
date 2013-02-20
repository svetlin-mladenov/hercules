#ifndef HERC_STRING_UTILS_H
#define HERC_STRING_UTILS_H

#include <stdbool.h>
#include <stdlib.h>

const char* string_drop_prefix(const char *prefix, const char *str);

bool string_array_member(const char **array, size_t n, const char *elem);

#endif
