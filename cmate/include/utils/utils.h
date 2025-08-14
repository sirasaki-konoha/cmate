#ifndef _UTILS_H
#define _UTILS_H
#include "term_color.h"

#ifndef NDEBUG
#define DBG(fmt, ...)                                    \
  do {                                                   \
    printf("%s " fmt, YELLOW("[DEBUG]"), ##__VA_ARGS__); \
  } while (0)
#else
#define DBG(fmt, ...) \
  do {                \
  } while (0)
#endif

/* impl at utils.c */
char *format_string(const char *restrict __format,
                    ...);         /* defined at line 13 */
int init_project();               /* defined at line 38 */
char *safe_strdup(const char *s); /* defined at line 58 */
char *join_strings(char **strings,
                   const char *separator); /* defined at line 80 */

/* impl at split_args.c */
char **split_args(const char *input);
void free_args(char **argv);

/* impl at guid.c */
char *generate_guid_v4();

/* impl at gen_makefile.c */
int get_array_len(char **array);

#endif
