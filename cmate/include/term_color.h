#ifndef __TERM_COLOR_H
#define __TERM_COLOR_H


#define RED(x) "\x1b[31m" x "\x1b[0m"
#define GREEN(x) "\x1b[32m" x "\x1b[0m"
#define YELLOW(x) "\x1b[33m" x "\x1b[0m"
#define BOLD(x) "\x1b[1m" x "\x1b[0m"

#define INFO(fmt, ...) printf("%s " fmt , GREEN(">>"), ##__VA_ARGS__)
#define ERR(fmt, ...) fprintf(stderr, "%s " fmt , RED("!!"), ##__VA_ARGS__)
#define WARN(fmt, ...)  fprintf(stderr, "%s " fmt , YELLOW("!!"), ##__VA_ARGS__)
#define DEBUG(fmt, ...) fprintf(stdout, "%s " fmt , "\x1b[36m??\x1b[0m", ##__VA_ARGS__)


#endif /* __TEMR_COLOR_H */
