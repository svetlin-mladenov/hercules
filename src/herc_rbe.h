/* RBE stands for report back end */

#ifndef HERC_RBE_H_
#define HERC_RBE_H_

#include "herr.h"

struct herc;
struct test_desc;

/* the interface all report back end must fallow */
struct herc_rbe {
	/* called when hercules start before any test has been run.This call is provided for consistency and is usually ignored */
	err_t (*start)(struct herc_rbe *, struct herc *);

	/* called when a new file containing tests has been found. */
	err_t (*start_file)(struct herc_rbe *, const char *filename);

	/* called before a suite is run */
	err_t (*start_suite)(struct herc_rbe *, const char *suitename);

	/* called before a test is run */
	err_t (*start_test)(struct herc_rbe *, struct test_desc *desc);

	/* called after a test is run */
	err_t (*end_test)(struct herc_rbe *, struct test_desc *desc);

	/* called after a suite is run */
	err_t (*end_suite)(struct herc_rbe *, struct herc *);

	/* called after all test in the current executable file has been run */
	err_t (*end_file)(struct herc_rbe *, struct herc *);

	/* called just before hercules ends execution. Must free any allocated resources */
	err_t (*end)(struct herc_rbe *, struct herc *);
};

#define herc_rbe_invoke(rbe, fun, ...) (rbe->fun != NULL) ? rbe->fun(rbe, ## __VA_ARGS__) : OK

err_t herc_make_default_rbe(struct herc_rbe **rbe, struct herc *h);

#endif /* HERC_RBE_H_ */
