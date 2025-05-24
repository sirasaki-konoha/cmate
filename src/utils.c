#include "file_io.h"
#include <embed_mkfile.h>
#include <gen_toml.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <toml.h>
#include <utils.h>

char *get_str(toml_table_t *conf, const char *name) {
  const char *raw_str = toml_raw_in(conf, name);

  if (raw_str) {
    char *unquoted;
    if (toml_rtos(raw_str, &unquoted) == 0) {
      return unquoted;
    }
  }

  return NULL;
}

char **get_arrays(toml_array_t *arr, int *out_count) {
  if (!arr)
    return NULL;

  int n = toml_array_nelem(arr);
  char **arrays = malloc(sizeof(char *) * (n + 1));
  if (!arrays)
    return NULL;

  int actual = 0;
  for (int i = 0; i < n; i++) {
    const char *raw = toml_raw_at(arr, i);
    if (!raw)
      continue;

    char *str;
    if (toml_rtos(raw, &str) == 0) {
      arrays[actual++] = str;
    }
  }

  arrays[actual] = NULL;
  if (out_count)
    *out_count = actual;

  return arrays;
}

void free_arrays(char **arr, int count) {
  if (!arr) return;
  for (int i = 0; i < count; i++) {
    free(arr[i]);
  }
  free(arr);
}

/**
 * Free an array of toml_parsed_t structures and their contents.
 * @param toml_parsed Array of toml_parsed_t
 * @param count       Number of elements in the array
 */
void free_toml_parsed(toml_parsed_t *toml_parsed, int count) {
  if (!toml_parsed) return;
  for (int i = 0; i < count; i++) {
    int n = 0;
    if (toml_parsed[i].libraries) {
      while (toml_parsed[i].libraries[n]) n++;
      free_arrays(toml_parsed[i].libraries, n);
    }
    n = 0;
    if (toml_parsed[i].includes) {
      while (toml_parsed[i].includes[n]) n++;
      free_arrays(toml_parsed[i].includes, n);
    }
    n = 0;
    if (toml_parsed[i].srcdirs) {
      while (toml_parsed[i].srcdirs[n]) n++;
      free_arrays(toml_parsed[i].srcdirs, n);
    }
    n = 0;
    if (toml_parsed[i].compile_file) {
      while (toml_parsed[i].compile_file[n]) n++;
      free_arrays(toml_parsed[i].compile_file, n);
    }
    free(toml_parsed[i].cflags);
    free(toml_parsed[i].compiler);
    free(toml_parsed[i].project_name);
  }
  free(toml_parsed);
}


char *format_string(const char *restrict __format, ...) {
  va_list args;
  va_start(args, __format);

  va_list args_copy;
  va_copy(args_copy, args);
  int len = vsnprintf(NULL, 0, __format, args_copy);
  va_end(args_copy);

  if (len < 0) {
    va_end(args);
    return NULL;
  }

  char *buf = malloc(len + 1);
  if (!buf) {
    va_end(args);
    return NULL;
  }

  vsnprintf(buf, len + 1, __format, args);
  va_end(args);
  return buf;
}

int init_project() {
  char *copy = malloc(template_project_toml_len + 1);
  memcpy(copy, template_project_toml, template_project_toml_len);
  copy[template_project_toml_len] = '\0';

  printf("=> project.toml ");
  if (create_and_write("project.toml", copy) != 0) {
    return 1;
  }
#ifdef _WIN32
  printf("OK\n");
#else
  printf("\x1b[32mOK\n\x1b[0m");
#endif

  free(copy);
  return 0;
}

/**
 * Safe string duplication function
 * @param s String to duplicate
 * @return Pointer to duplicated string, NULL on error
 */
char *safe_strdup(const char *s) {
  if (!s)
    return NULL;

  char *dup = malloc(strlen(s) + 1);
  if (!dup) {
    fprintf(stderr, "Memory allocation failed\n");
    return NULL;
  }

  strcpy(dup, s);
  return dup;
}
