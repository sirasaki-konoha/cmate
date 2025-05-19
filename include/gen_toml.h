#ifndef _GEN_TOML_h
#define _GEN_TOML_h
#include <toml.h>

typedef struct {
  char *project_name;
  char *cflags;
  char *compiler;
  char **compile_file;
  char **srcdirs;
  char **includes;
  char **libraries;
} toml_parsed_t;

char **get_arrays(toml_array_t *arr, int *out_cound);

char *get_str(toml_table_t *conf, const char *name);

void free_arrays(char **arr, int count);

void free_toml_parsed(toml_parsed_t toml_parsed);

#endif /* _GEN_TOML_h */
