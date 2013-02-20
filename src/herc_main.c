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

#include "test_finder.h"
#include "test_runner.h"
#include "test_desc.h"

#include "logger.h"
#include "herr.h"

#include "branch_annotation.h"

#include "herc_string_utils.h"

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

err_t herc_make(struct herc **h, int argc, char **argv) {
	*h = calloc(1, sizeof(struct herc));
	if (*h==NULL) {
		return err_nomem();
	}

	return parse_cdm_args(*h, argc, argv);
}

int herc_run(struct herc *h, int argc, char **argv) {
	const char *target;
        int err, ret;
	
	target = argv[0];

	err = run_tests_in(h, target);
	if (err < 0) {
		ret = err;
		cprint(CPRINT_RED, "Unexpected error while trying to execute the tests\n");
	} else if (h->run.total == 0) {
		printf("No tests found in %s\n", target);
		ret = 0;
	} else {
		ret = h->run.failing;
		cprint(h->run.failing == 0 ? CPRINT_GREEN : CPRINT_RED,
			"All tests: %-10dPassed Tests: %-10dFailed Tests: %-10dFiltered out Tests: %-10d\n", h->run.total, h->run.passing, h->run.failing, h->run.filtered);
	}

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

	err =  try_running_tests_in_dso(h, file, handle);

	dlclose(handle);
	return err;
}

int run_test(struct herc *h, void *, test_desc *);

int try_running_tests_in_dso(struct herc *h, const char *file, void *handle) {
	struct cr_list *tests = get_testnames_in_file(file);
	
	cr_list_iter *iter = cr_list_iter_create(tests);
	while (!cr_list_iter_past_end(iter)) {
		test_desc *test = (test_desc*)cr_list_iter_get(iter);
		run_test(h, handle, test); 
		cr_list_iter_next(iter);
	}
	cr_list_iter_free(iter);


	cr_list_free(tests);
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

int run_test(struct herc *h, void *handle, test_desc *desc) {
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
		err = test_runner_run_test(test);
		gettimeofday(&tv_end, NULL);
		ellapsed_time = (float)(tv_end.tv_sec - tv_start.tv_sec) + (tv_end.tv_usec - tv_start.tv_usec)/1000000.0;
		if (err < 0) {
			cprint(CPRINT_RED, "Running test %s::%s... [%.2f s] %s\n", desc->suit, desc->test, ellapsed_time, test_runner_get_fail_msg());
			h->run.failing++;
		} else {
			printf("Running test %s::%s... [%.2f s] OK\n", desc->suit, desc->test, ellapsed_time);
			h->run.passing++;
		}
	} else {
		h->run.total++;
		h->run.filtered++;
	}

	return 0;
}

