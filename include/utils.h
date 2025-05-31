#ifndef _UTILS_H
#define _UTILS_H

/* impl at utils.c */
char *format_string(const char *restrict __format, ...); /* defined at line 13 */
int init_project(); /* defined at line 38 */
char *safe_strdup(const char *s); /* defined at line 58 */

/* impl at run_command.c */
int run_command(const char *cmd, char *const argv[]);

/* impl at split_args.c */
char **split_args(const char *input);
void free_args(char **argv);



#endif
