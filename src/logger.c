#include "logger.h"

#include "herr.h"

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>

static void logger_set_use_color(bool new_use_color); 
static bool logger_should_use_color();
static void logger_auto_use_color();

/* PUBLIC API */

err_t logger_use_color(const char *str) {
	if (strcmp("always", str)==0) {
		logger_set_use_color(true);
	} else if (strcmp("never", str)==0) {
		logger_set_use_color(false);
	} else if (strcmp("auto", str)==0) {
		logger_auto_use_color();
	} else {
		return err_general(NULL, "Invalide value for color: %s. Allowed values are 'always', 'never'", str);
	}

	return OK;
}

void cprint(const char *color, const char *fmt, ...) {
        va_list args;
        va_start(args, fmt);

        if (logger_should_use_color()) {
                fputs(color, stdout);
                vprintf(fmt, args);
                fputs(CPRINT_STOP, stdout);
        } else {
                vprintf(fmt, args);
        }

        va_end(args);
}

void log_debug(const char *fmt, ...) {
        va_list args;
        va_start(args, fmt);

        vprintf(fmt, args);
        fputc('\n', stdout);
        
        va_end(args);
}

/* PRIVATE API */

static unsigned use_color = 0xFF00;

static bool logger_should_use_color() {
        if ((use_color & 0xFF00) != 0) {
                //this is the first time we call some of the logger functions and it hast been yet initialied
                logger_auto_use_color();
        }
        return use_color;
}

static void logger_set_use_color(bool new_use_color) {
        use_color = (unsigned)new_use_color;
}

static void logger_auto_use_color() {
        //TODO implement
	logger_set_use_color(true);
}
