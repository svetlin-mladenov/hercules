#include "herr.h"
#include "herc_string_utils.h"
#include "herc_main.h"
#include "logger.h"

#include <stdlib.h>


err_t parse_cdm_args(struct herc *h, int argc, char **argv) {
        size_t i;
        const char *arg;
        err_t err;

        for (i = 1; i<argc; i++) {
                err = OK;
                if ( (arg = string_drop_prefix("--color=", argv[i])) ) {
                        err = logger_use_color(arg);
                } else {
                        err = err_general(NULL, "unknown argument %s", argv[i]);
                }
                if (err) {
                        return err_general(err, "Could not parse command line arguments");
                }
        }

        return OK;
}
