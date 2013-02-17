#ifndef HERR_H
#define HERR_H

struct err_error {
	char * msg;
	struct err_error *cause;
	unsigned char allocated;
};

typedef struct err_error* err_t;

#define OK NULL

#define err_general(cause, fmt, ...) err_new(cause, fmt, ##__VA_ARGS__)

void err_print(err_t );

void err_free(err_t );

err_t err_new(err_t cause, const char *fmt, ...);

#endif
