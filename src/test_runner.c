#include "test_runner.h"

#include <setjmp.h>

jmp_buf abort_test_point;

/*TODO move to a separte file*/
const char *errmsg; /*TODO used a seperate buffer */

int test_runner_run_test(test_fn test) {
        int v = setjmp(abort_test_point);
        if (v) {
                /*the test has failed*/
                return -1;
        } else {
                test();
                return 0;
        }
}

void herc_fail(const char *msg) {
        errmsg = msg;
        longjmp(abort_test_point, 1);
}

/*TODO better is for test_runner_run_test to return the err msg*/
const char *test_runner_get_fail_msg() {
        return errmsg;
}
