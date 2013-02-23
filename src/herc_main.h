#ifndef HERC_MAIN_H
#define HERC_MAIN_H

#include <stdlib.h>

struct herc_rbe;

struct herc {

	struct {
		size_t total;
		size_t failing;
		size_t passing;
		size_t filtered;
	} run;
	
	struct {
		struct {
			char **array;
			size_t n;
		} suites;
		struct {
			char **array;
			size_t n;
		} tests;
	} filter;

	struct herc_rbe *rbe;
};

int herc_main(int, char **);

#endif
