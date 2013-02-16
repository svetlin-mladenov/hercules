#include "herr.h"

#define _GNU_SOURCE
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

#include "branch_annotation.h"

static err_t err_emergency_nomem();
static err_t err_emergency_bad_format();

/* PUBLIC API */

void err_print(err_t err) {
	fputs(err->msg, stderr);
	fputc('\n', stderr);
}

void err_free(err_t err) {
	if (err->allocated) {
		free(err->msg);
	}
}

err_t err_new(const char *fmt, ...) {
	char *msg;
	va_list args;
	int ret;

	va_start(args, fmt);
	ret = vasprintf(&msg, fmt, args);
	va_end(args);
	if (ret < 0) {
		return err_emergency_bad_format(fmt);
	}

	struct err_error *err = malloc(sizeof(struct err_error));
	if (unlikely(err == NULL)) {
		free(msg);
		return err_emergency_nomem();
	}

	err->msg = msg;
	err->allocated = 1;
	return err;
}

/* PRIVATE FUNCTIONS */

static err_t err_emergency_nomem() {
	static struct err_error err = {
		.msg = "No memory",
		.allocated = 0
	};
	return &err;
}

static err_t err_emergency_bad_format(const char *fmt) {
	return err_new("An unknown error occured while trying to create an error with error format message %s", fmt);
}
