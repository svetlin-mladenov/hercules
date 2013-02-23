#include "herc_rbe_cout.h"

#include <stdio.h>

#include "logger.h"
#include "test_desc.h"
#include "halloc.h"

static err_t cout_end_test(struct herc_rbe *rbe, struct test_desc *desc) {
	if (desc->status == FAILED) {
		cprint(CPRINT_RED, "Running test %s::%s... [%.2f s] %s\n", desc->suit, desc->test, desc->ellapsed_time, desc->fail_msg);
	} else if (desc->status == PASSED) {
		printf("Running test %s::%s... [%.2f s] OK\n", desc->suit, desc->test, desc->ellapsed_time);
	}
	return OK;
}

static err_t cout_end_file(struct herc_rbe *rbe, struct herc *h) {
	cprint(h->run.failing == 0 ? CPRINT_GREEN : CPRINT_RED,
				"All tests: %-10dPassed Tests: %-10dFailed Tests: %-10dFiltered out Tests: %-10d\n", h->run.total, h->run.passing, h->run.failing, h->run.filtered);
	return OK;
}

err_t herc_rbe_cout_make(struct herc_rbe **rbe, struct herc *h) {
	err_t err;
	struct herc_rbe *cout;

	ERR_RET(halloc(cout, sizeof(struct herc_rbe)));
	cout->end_test = cout_end_test;
	cout->end_file = cout_end_file;

	*rbe = cout;
	return OK;
}
