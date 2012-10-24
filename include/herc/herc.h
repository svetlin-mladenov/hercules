#ifndef HERC_HERC_H
#define HERC_HERC_H

#include <stdlib.h>

#define _HERC_CASE_NAME_DELIMITER _D5a_

//NOTE if you change the manglin schema you must also change the demangling schema in hercules-source-tree/src/mangler.c
#define _HERC_MANGLE(test_case, delimiter, test_name) __HtEsT_ ## test_case ## _D5a_ ## test_name

#define HTEST(test_case, test_name) void  _HERC_MANGLE(test_case, _HERC_CASE_NAME_DELIMITER, test_name) ()

/*************
 * assertions *
 *************/

#define assert_true(expr) if (!(expr)) {herc_fail("\"" #expr "\"" " expected to be true but was false");}

#define assert_not_null(expr) if ((expr)==NULL) {herc_fail("\"" #expr "\"" " expected to be NULL but it isn't");}

void herc_fail(const char *);

#endif 
