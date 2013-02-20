#include "herr.h"
#include "herc_string_utils.h"
#include "herc_main.h"
#include "logger.h"

#include <stdlib.h>
#include <string.h>

static err_t parse_suites(struct herc *h, const char *arg);
static err_t parse_tests(struct herc *h, const char *arg);


err_t parse_cdm_args(struct herc *h, int argc, char **argv) {
        size_t i;
        const char *arg;
        err_t err;

        for (err = OK, i = 1; i<argc; i++) {
                if ( (arg = string_drop_prefix("--color=", argv[i])) != NULL ) {
                        err = logger_use_color(arg);
                } else if ( (arg = string_drop_prefix("--suites=", argv[i])) != NULL) {
			err = parse_suites(h, arg);
		} else if ( (arg = string_drop_prefix("--tests=", argv[i])) != NULL) {
			err = parse_tests(h, arg);
		} else {
                        err = err_general(NULL, "unknown argument %s", argv[i]);
                }

                if (err) {
                        return err_general(err, "Could not parse command line arguments");
                }
        }

        return OK;
}

/* PRIVATE API */

#define __CSV_ITERATE_OVER_VALUES__(OP) \
     for (i = 0, len=0; arg[i]; i++) { \
                for (; arg[i]!='\0' && arg[i]==','; i++) \
                        ; \
                for(begin = i; arg[i]!='\0' && arg[i]!=','; i++) \
                        ; \
                len = i - begin; \
                if (len > 0) { \
			OP; \
                } \
        }

static err_t parse_csv_string(char ***res, size_t *res_n, const char *arg) {
	size_t i, begin, len, count, next;
	char **array;

	count=0;
	__CSV_ITERATE_OVER_VALUES__(count++);
	if (count <= 0) {
		*res_n = 0;
		*res = NULL;
	} else {
		*res_n = count;
		array = calloc(count, sizeof(const char *));
		if (array == NULL)
			return err_nomem();
		*res = array;
		next = 0;
		__CSV_ITERATE_OVER_VALUES__( 
				array[next] = malloc(len+1);
				strncpy(array[next], arg + begin, len);
				next++;
		);
	}
	return OK;
}

#undef __CSV_ITERATE_OVER_VALUES__

static err_t parse_suites(struct herc *h, const char *arg) {
	char **res;
	size_t n;
	err_t err;

	ERR_RET(parse_csv_string(&res, &n, arg));

	h->filter.suites.array = res;
	h->filter.suites.n = n;
	return OK;
}

static err_t parse_tests(struct herc *h, const char *arg) {
	char **res;
	size_t n;
	err_t err;

	ERR_RET(parse_csv_string(&res, &n, arg));

	h->filter.tests.array = res;
	h->filter.tests.n = n;
	return OK;
}
