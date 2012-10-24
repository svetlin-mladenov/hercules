#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <dlfcn.h>

#include "list_testname.h"
#include "test_finder.h"
#include "test_runner.h"

int run_tests_in(const char *, unsigned *, unsigned *);
int try_running_tests_in_dso(const char *, void *, unsigned *, unsigned *);
int try_running_tests_in_file(const char *, unsigned *, unsigned *);
int try_running_tests_in_dir(const char *, unsigned *, unsigned *);
struct list_testname *get_testnames_in_file(const char *);

#define CPRINT_RED   "\033[31m"
#define CPRINT_GREEN "\033[32m"
#define CPRINT_STOP  "\033[0m"

#define cprint(color, msg, ...) printf("%s" msg CPRINT_STOP, (color), ##__VA_ARGS__) 

#define log_debug(msg, ...) printf(msg "\n", ##__VA_ARGS__)

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
		cprint(CPRINT_RED, "Error while trying to execute the tests\n");
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
		fprintf(stderr, "%s\n",  strerror(errno));
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
		log_debug("Trying to dso load %s failed. Probably not a DSO", file);
		return 0;
	}

	err =  try_running_tests_in_dso(file, handle, failing_tests, tests_count);

	dlclose(handle);
	return err;
}

int run_test(void *, const char *, unsigned *, unsigned *);

int try_running_tests_in_dso(const char *file, void *handle, unsigned *failing_tests, unsigned *tests_count) {
	struct list_testname *tests = get_testnames_in_file(file);
	
	struct list_testname_item *test = list_testname_first(tests);

	while (test != NULL) {
		run_test(handle, test->testname, failing_tests, tests_count); 
		test = list_testname_next(test);
	}


	list_testname_free(tests);
	return 0;
}

int run_test(void *handle, const char *test_name, unsigned *failing_tests, unsigned *tests_count) {
	test_fn test = (test_fn)(intptr_t) dlsym(handle, test_name);
	if (test == NULL) {
		fprintf(stderr, "error while looking up symbol: %s\n", dlerror());
		return -1;
	}	

	printf("Running test %s... ", test_name);
	(*tests_count)++;
	if (test_runner_run_test(test) < 0) {
		printf("%s \n", test_runner_get_fail_msg());
		(*failing_tests)++;
	} else {
		printf("OK\n");
	}

	return 0;
}

