#define _GNU_SOURCE

#include "test_runner.h"

#include <setjmp.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

jmp_buf abort_test_point;

char *errmsg = NULL;

int test_runner_run_test(test_fn test) {
	if (errmsg != NULL) {
		free(errmsg);
		errmsg = NULL;
	}

        int v = setjmp(abort_test_point);
        if (v) {
                /*the test has failed*/
                return -1;
        } else {
                test();
                return 0;
        }
}

void herc_fail(const char *msg, ...) {
	if (errmsg != NULL) {
		fprintf(stderr, "UNEXPECTED STATE: failing twice in the same method. That could be a bug in Hercules or in you client application.\n");
		/* may be free errmsg here just in case */
		errmsg = NULL;
	}
	
	va_list ap;
	va_start(ap, msg);
	if (vasprintf(&errmsg, msg, ap) < 0) {
		fprintf(stderr, "Formatting the fail messaged failed. The message is %s\n", msg);
		errmsg = NULL;
	}
	va_end(ap);
        
        longjmp(abort_test_point, 1);
}

/*TODO better is for test_runner_run_test to return the err msg*/
const char *test_runner_get_fail_msg() {
        return errmsg;
}
