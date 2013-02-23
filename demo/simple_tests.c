#include <stdio.h>
#include <stdlib.h>

#include <herc/herc.h>

HTEST(test_case, empty_test) {
	/*success*/
}

HTEST(test_case, fun_assert_pass) {
	int a = 1;
	assert_true(a);
}

HTEST(test_case, fun_assert_fail) {
	int a = 0;
	assert_true(a);
}

HTEST(str_case, assert_eq_str_pass) {
	assert_eq_str("aaaa", "aaaa");
}

HTEST(str_case, assert_eq_str_fail) {
	assert_eq_str("str1", "str2");
}
