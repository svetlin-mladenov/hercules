#ifndef TEST_RUNNER_H
#define TEST_RUNNER_H

#include "herc/herc.h"

typedef void (*test_fn) (void);

int test_runner_run_test(test_fn test);

const char *test_runner_get_fail_msg();

#endif
