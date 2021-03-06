#include "herc_main.h"

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>

#include <dlfcn.h>

#include <cryad/list.h>
#include <cryad/slist.h>

#include "table.h"

#include "test_finder.h"
#include "test_runner.h"
#include "test_desc.h"
#include "suite_desc.h"

#include "logger.h"
#include "herr.h"

#include "branch_annotation.h"

#include "herc_string_utils.h"
#include "herc_rbe.h"

int herc_run(struct herc *h, int argc, char **argv);
int run_tests_in(struct herc *h, const char *);
int try_running_tests_in_dso(struct herc *h, const char *, void *);
int try_running_tests_in_file(struct herc *h, const char *);
int try_running_tests_in_dir(struct herc *h, const char *);

err_t parse_cdm_args(struct herc *h, int argc, char **argv);
err_t herc_make(struct herc **h, int argc, char **argv);

int herc_main(int argc, char **argv) {
	err_t err;
	struct herc *h;

	err = herc_make(&h, argc, argv);
	if (err) {
		err_print(err);
		err_free(err);
		return 1;
	}

	return herc_run(h, argc, argv);
}

err_t herc_make(struct herc **h_res, int argc, char **argv) {
	err_t err;
	struct herc *h;
	*h_res = NULL;

	h = calloc(1, sizeof(struct herc));
	if (h==NULL) {
		err = err_nomem();
	} else if ( (err = parse_cdm_args(h, argc, argv)) ) {
		free(h);
	} else if ( (err = herc_make_default_rbe(&(h->rbe), h)) ) {
		free(h);
	} else {
		*h_res = h;
	}

	return err;
}

int herc_run(struct herc *h, int argc, char **argv) {
	const char *target;
    int err, ret;
	
	target = argv[0];

	herc_rbe_invoke(h->rbe, start, h);
	err = run_tests_in(h, target);
	if (err < 0) {
		ret = err;
		cprint(CPRINT_RED, "Unexpected error while trying to execute the tests\n");
	} else if (h->run.total == 0) {
		printf("No tests found in %s\n", target);
		ret = 0;
	} else {
		ret = h->run.failing;
	}
	herc_rbe_invoke(h->rbe, end, h);

	return ret;
}

int run_tests_in(struct herc *h, const char *target) {
	struct stat target_stat;
	int err;

	//always return defined values.
	h->run.failing = 0;
	h->run.total = 0;

	err = stat(target, &target_stat);
	if (err < 0) {
		fprintf(stderr, "Stating %s failed: %s\n", target, strerror(errno));
		return -1;
	}

	if (S_ISDIR(target_stat.st_mode)) {
		return try_running_tests_in_dir(h, target);
	} else if (S_ISREG(target_stat.st_mode)) {
		return try_running_tests_in_file(h, target);
	}

	//not a dir neither a regular file
	fprintf(stderr, "%s is not a regular file or a directory", target);
	return -1;
}

int try_running_tests_in_dir(struct herc *h, const char *target) {
	DIR *dir;
	struct dirent *dirent;
	
	dir = opendir(target);
	if (dir == NULL) {
		fprintf(stderr, "error opening directory %s\n", target);
		return -1;
	}

	while ((dirent = readdir(dir)) != NULL) {
		try_running_tests_in_file(h, dirent->d_name);
	}

	closedir(dir);
	return 0;
}

void *dlopen_file(const char *filename, int flag) {
	return dlopen(NULL, flag);
	/*char *rel_filename;
	void *handle;
	if(strchr(filename, '/') == NULL) {
		// filename is NOT a path so dlopen will try to search for it as if it's a shared library
		// using the same search pattern as ld however we don't what this behavour so we go around it
		// by prepending a ./ to filename
		rel_filename = malloc(strlen(filename) + 3);
		strcpy(rel_filename, "./"),
		strcat(rel_filename, filename);
		handle = dlopen(rel_filename, flag);
		free(rel_filename);
		return handle;
	} else {
		// filename is relative or absolute path and dlopen will search for it in the current dir
		// so there is no need to do anything special except just calling dlopen
		return dlopen(filename, flag);
	}*/
}

int try_running_tests_in_file(struct herc *h, const char *file) {
	void *handle;
	int err;

	handle = dlopen_file(file, RTLD_NOW);
	if (handle == NULL) {
		log_debug("Trying to load %s failed: %s\n.", file, dlerror());
		return 0;
	}

	herc_rbe_invoke(h->rbe, start_file, file);
	err =  try_running_tests_in_dso(h, file, handle);
	herc_rbe_invoke(h->rbe, end_file, h);

	dlclose(handle);
	return err;
}

int run_test(struct herc *h, void *, test_desc *, void *);

static void *invoke_before_suite(suite_desc *suite) {
	if (suite && suite->before_suite) {
		return suite->before_suite(NULL);//pass default fixture just for to simplify implementation
	} else {
		return NULL;
	}
}

static void invoke_after_suite(suite_desc *suite, void *suite_fixture) {
	if (suite && suite->after_suite)
		suite->after_suite(suite_fixture);
}

static void *invoke_before_test(suite_desc *suite, void *suite_fixture) {
	if (suite && suite->before_test) {
		return suite->before_test(suite_fixture);
	} else {
		return suite_fixture;
	}
}

static void invoke_after_test(suite_desc *suite, void *test_fixture) {
	if (suite && suite->after_test)
		suite->after_test(test_fixture);
}

int try_running_tests_in_dso(struct herc *h, const char *file, void *handle) {
	size_t i = 0;
	void *suite_fixture, *test_fixture;
	//get the suites
	Table_T suitesTable = get_testnames_in_file(file, handle);

	//convert to array for easier sorting and iteration
	unsigned nSuites = Table_length(suitesTable);
	void **suites = Table_toArray(suitesTable, 0);
	Table_free(&suitesTable);
	
	//sort
	qsort(suites, nSuites, 2*sizeof(suites[0]), (int (*)(const void *, const void *))strcmp);

	//iterate and run
	for (i = 0; i<nSuites; i++) {
		suite_desc *suite = suites[i*2+1];
		suite_fixture = invoke_before_suite(suite);//TODO call RBE callback
		cr_list_iter *iter = cr_list_iter_create(suite->tests);
		while (!cr_list_iter_past_end(iter)) {
			test_fixture = invoke_before_test(suite, suite_fixture);
			test_desc *test = (test_desc*)cr_list_iter_get(iter);
			herc_rbe_invoke(h->rbe, start_test, test);
			run_test(h, handle, test, test_fixture);
			herc_rbe_invoke(h->rbe, end_test, test);
			invoke_after_test(suite, test_fixture);
			cr_list_iter_next(iter);
		}
		cr_list_iter_free(iter);
		invoke_after_suite(suite, suite_fixture);
	}

	for (i = 0; i<nSuites; i++) {
		suite_desc_free(suites[i*2+1]);
	}


	free(suites);
	return 0;
}

bool does_testsuite_pass_filter(struct herc *h, test_desc *desc) {
	return h->filter.suites.n == 0 || string_array_member((const char **)h->filter.suites.array, h->filter.suites.n, desc->suit);
}

bool does_testname_pass_filter(struct herc *h, test_desc *desc) {
	return h->filter.tests.n == 0 || string_array_member((const char **)h->filter.tests.array, h->filter.tests.n, desc->test);
}

bool does_test_pass_filter(struct herc *h, test_desc *desc) {
	return does_testsuite_pass_filter(h, desc) && does_testname_pass_filter(h, desc);
}

int run_test(struct herc *h, void *handle, test_desc *desc, void *fixture) {
	struct timeval tv_start, tv_end;
	double ellapsed_time;
	int err = 0;

	if (does_test_pass_filter(h, desc)) {
		test_fn test = (test_fn)(intptr_t) dlsym(handle, desc->symname);
		if (test == NULL) {
			fprintf(stderr, "error while looking up symbol: %s\n", dlerror());
			return -1;
		}	

		h->run.total++;
		gettimeofday(&tv_start, NULL);
		err = test_runner_run_test(test, fixture);
		gettimeofday(&tv_end, NULL);
		ellapsed_time = (float)(tv_end.tv_sec - tv_start.tv_sec) + (tv_end.tv_usec - tv_start.tv_usec)/1000000.0;
		desc->status = (err < 0) ? FAILED : PASSED;
		desc->ellapsed_time = ellapsed_time;
		desc->fail_msg = test_runner_get_fail_msg();
		if (err < 0) {
			h->run.failing++;
		} else {
			h->run.passing++;
		}
	} else {
		h->run.total++;
		h->run.filtered++;
	}

	return 0;
}

