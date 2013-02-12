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

void *dlopen_file(const char *filename, int flag) {
	char *rel_filename;
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
	}
}

int try_running_tests_in_file(const char *file, unsigned *failing_tests, unsigned *tests_count) {
	void *handle;
	int err;

	handle = dlopen_file(file, RTLD_NOW);
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
	struct timeval tv_start, tv_end;
	double ellapsed_time;
	int err = 0;

	test_fn test = (test_fn)(intptr_t) dlsym(handle, desc->symname);
	if (test == NULL) {
		fprintf(stderr, "error while looking up symbol: %s\n", dlerror());
		return -1;
	}	

	(*tests_count)++;
	gettimeofday(&tv_start, NULL);
	err = test_runner_run_test(test);
	gettimeofday(&tv_end, NULL);
	ellapsed_time = (float)(tv_end.tv_sec - tv_start.tv_sec) + (tv_end.tv_usec - tv_start.tv_usec)/1000000.0;
	if (err < 0) {
		cprint(CPRINT_RED, "Running test %s::%s... [%.2f s] %s\n", desc->suit, desc->test, ellapsed_time, test_runner_get_fail_msg());
		(*failing_tests)++;
	} else {
		printf("Running test %s::%s... [%.2f s] OK\n", desc->suit, desc->test, ellapsed_time);
	}

	return 0;
}

