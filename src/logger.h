#ifndef LOGGER_H
#define LOGGER_H

#define CPRINT_RED   "\033[31m"
#define CPRINT_GREEN "\033[32m"
#define CPRINT_STOP  "\033[0m"

#define cprint(color, msg, ...) printf("%s" msg CPRINT_STOP, (color), ##__VA_ARGS__) 

#define log_debug(msg, ...) printf(msg "\n", ##__VA_ARGS__)

#endif
