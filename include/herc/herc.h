#ifndef HERC_HERC_H
#define HERC_HERC_H

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define _HERC_CASE_NAME_DELIMITER      _D5a_
#define _HERC_CASE_NAME_DELIMITER_STR "_D5a_"

/*NOTE if you change the manglin schema you must also change the demangling schema in hercules-source-tree/src/mangler.c*/
#define _HERC_MANGLE(test_case, delimiter, test_name) __HtEsT_ ## test_case ## _D5a_ ## test_name

#define HTEST(test_case, test_name) void  _HERC_MANGLE(test_case, _HERC_CASE_NAME_DELIMITER, test_name) ()

void herc_fail(const char *, ...);

#define _HERC_STR_EXPR(expr) "\"" #expr "\""

/*************
 * assertions *
 *************/

#define assert_true(expr) if (!(expr)) {herc_fail(_HERC_STR_EXPR(expr) " expected to be true but was false");}

#define assert_not_null(expr) if ((expr)==NULL) {herc_fail(_HERC_STR_EXPR(expr) " expected NOT to be NULL but it is");}

#define assert_null(expr) if ((expr)!=NULL) {herc_fail(_HERC_STR_EXPR(expr) " expected to be NULL but it isn't");}

#define assert_eq(exp, act) if ((exp)!=(act)) {herc_fail(_HERC_STR_EXPR(act) " expected to be equal to " _HERC_STR_EXPR(exp) " but it isn't");}

#define assert_eqi(exp, act) \
	do {\
		intmax_t exp_val = (exp); \
		intmax_t act_val = (act); \
		if (exp_val != act_val) {\
			herc_fail(_HERC_STR_EXPR(act) "(value: %jd) expected to be equal to " _HERC_STR_EXPR(exp) "(value: %jd) but it isn't", act_val, exp_val); \
		} \
	} while(0);

#define assert_eqmem(ptr_exp, ptr_act, len) \
	do { \
		if (memcmp(ptr_exp, ptr_act, len) != 0) {\
			herc_fail("Memeory at " _HERC_STR_EXPR(ptr_exp) "(addr: %p, len: %d) is no equal to memory at " _HERC_STR_EXPR(ptr_act) "(addr: %p, len: %d)", ptr_exp, len, ptr_act, len); \
		}\
	} while(0);

#define assert_eq_str(exp, act) \
	do { \
		if (strcmp(exp, act)!=0) { \
			herc_fail("Expected: \"%s\" but actual: \"%s\"", exp, act); \
		} \
	} while(0);


#endif 
