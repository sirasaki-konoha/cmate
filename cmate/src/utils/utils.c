#include "utils/utils.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "file_io/file_io.h"
#include "makefile/embed_mkfile.h"
#include "utils/term_color.h"

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
  if (!s) return NULL;

  char *dup = malloc(strlen(s) + 1);
  if (!dup) {
    fprintf(stderr, "Memory allocation failed\n");
    return NULL;
  }

  strcpy(dup, s);
  return dup;
}

/**
 * Joins an array of strings into a single string, inserting the specified
 * separator between each string.
 *
 * @param strings    A NULL-terminated array of strings to be joined.
 * @param separator  The string to insert between each pair of strings.
 * @return           A newly allocated string containing the joined result, or
 * NULL on allocation failure. The caller is responsible for freeing the
 * returned string.
 */
char *join_strings(char **strings, const char *separator) {
  unsigned long total_length = 0;
  int i;
  for (i = 0; strings[i] != NULL; i++) {
    total_length += strlen(strings[i]);
  }
  total_length += strlen(separator) * (i - 1);

  char *result = malloc(total_length + 1);
  result[0] = '\0';

  for (int i = 0; strings[i] != NULL; i++) {
    strcat(result, strings[i]);
    if (strings[i + 1] != NULL) {
      strcat(result, separator);
    }
  }

  return result;
}
