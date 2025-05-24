#ifndef _UTILS_H
#define _UTILS_H

char *format_string(const char *restrict __format, ...);
int init_project();

int run_command(const char *cmd, char *const argv[]);
char **split_args(const char *input);

char *safe_strdup(const char *s);
void free_args(char **argv);

int get_array_len(char **array);


#endif
