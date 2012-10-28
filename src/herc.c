#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <dlfcn.h>

#include <cryad/list.h>
#include <cryad/slist.h>

#include "test_finder.h"
#include "test_runner.h"
#include "test_desc.h"

#include "logger.h"

int run_tests_in(const char *, unsigned *, unsigned *);
int try_running_tests_in_dso(const char *, void *, unsigned *, unsigned *);
int try_running_tests_in_file(const char *, unsigned *, unsigned *);
int try_running_tests_in_dir(const char *, unsigned *, unsigned *);

int main(int argc, char **argv) {
	const char *target;
	unsigned failing_tests, tests_count;
	int err, ret;
	
	if (argc > 1) {
		target = argv[1];
	} else {
		target = ".";
	}	

	err = run_tests_in(target, &failing_tests, &tests_count);
	if (err < 0) {
		ret = err;
		cprint(CPRINT_RED, "Unexpected error while trying to execute the tests\n");
	} else if (tests_count == 0) {
		printf("No tests found in %s\n", target);
		ret = 0;
	} else {
		ret = failing_tests;
		cprint(failing_tests == 0 ? CPRINT_GREEN : CPRINT_RED,
			"All tests: %-10dPassed Tests: %-10dFailed Tests: %-10d\n", tests_count, tests_count - failing_tests, failing_tests);
	}

	return ret;
}

int run_tests_in(const char *target, unsigned *failing_tests, unsigned *tests_count) {
	struct stat target_stat;
	int err;

	//always return defined values.
	*failing_tests = 0;
	*tests_count = 0;

	err = stat(target, &target_stat);
	if (err < 0) {
		fprintf(stderr, "Stating %s failed: %s\n", target, strerror(errno));
		return -1;
	}

	if (S_ISDIR(target_stat.st_mode)) {
		return try_running_tests_in_dir(target, failing_tests, tests_count);
	} else if (S_ISREG(target_stat.st_mode)) {
		return try_running_tests_in_file(target, failing_tests, tests_count);
	}

	//not a dir neither a regular file
	fprintf(stderr, "%s is not a regular file or a directory", target);
	return -1;
}

int try_running_tests_in_dir(const char *target, unsigned *failing_tests, unsigned *tests_count) {
	DIR *dir;
	struct dirent *dirent;
	
	dir = opendir(target);
	if (dir == NULL) {
		fprintf(stderr, "error opening directory %s\n", target);
		return -1;
	}

	while ((dirent = readdir(dir)) != NULL) {
		try_running_tests_in_file(dirent->d_name, failing_tests, tests_count);
	}

	closedir(dir);
	return 0;
}

int try_running_tests_in_file(const char *file, unsigned *failing_tests, unsigned *tests_count) {
	void *handle;
	int err;

	handle = dlopen(file, RTLD_NOW);
	if (handle == NULL) {
		log_debug("Trying to load %s failed: %s\n.", file, dlerror());
		return 0;
	}

	err =  try_running_tests_in_dso(file, handle, failing_tests, tests_count);

	dlclose(handle);
	return err;
}

int run_test(void *, test_desc *, unsigned *, unsigned *);

int try_running_tests_in_dso(const char *file, void *handle, unsigned *failing_tests, unsigned *tests_count) {
	struct cr_list *tests = get_testnames_in_file(file);
	
	cr_list_iter *iter = cr_list_iter_create(tests);
	while (!cr_list_iter_past_end(iter)) {
		test_desc *test = (test_desc*)cr_list_iter_get(iter);
		run_test(handle, test, failing_tests, tests_count); 
		cr_list_iter_next(iter);
	}
	cr_list_iter_free(iter);


	cr_list_free(tests);
	return 0;
}

int run_test(void *handle, test_desc *desc, unsigned *failing_tests, unsigned *tests_count) {
	test_fn test = (test_fn)(intptr_t) dlsym(handle, desc->symname);
	if (test == NULL) {
		fprintf(stderr, "error while looking up symbol: %s\n", dlerror());
		return -1;
	}	

	(*tests_count)++;
	if (test_runner_run_test(test) < 0) {
		cprint(CPRINT_RED, "Running test %s::%s... %s\n", desc->suit, desc->test, test_runner_get_fail_msg());
		(*failing_tests)++;
	} else {
		printf("Running test %s::%s... OK\n", desc->suit, desc->test);
	}

	return 0;
}

