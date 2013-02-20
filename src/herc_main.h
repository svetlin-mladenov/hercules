#ifndef HERC_MAIN_H
#define HERC_MAIN_H

#include <stdlib.h>

struct herc {

	struct {
		size_t total;
		size_t failing;
		size_t passing;
	} run;

	char **suites_filter;
	char **tests_filter;
};

int herc_main(int, char **);

#endif
