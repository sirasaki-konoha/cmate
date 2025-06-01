#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tomlc99/toml.h"

#include "file_io.h"
#include "term_color.h"
#include "embed_mkfile.h"
#include "toml_utils.h"
#include "utils.h"

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
  char *copy = malloc(template_cmate_len + 1);
  memcpy(copy, template_cmate, template_cmate_len);
  copy[template_cmate_len] = '\0';

  INFO("Cmate.toml: ");
  if (create_and_write("Cmate.toml", copy) != 0) {
    return 1;
  }
  printf("OK\n");

  free(copy);
  return 0;
}

/**
 * Safe string duplication function
 * @param s String to duplicate
 * @return Pointer to duplidcate string, NULL on error
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
