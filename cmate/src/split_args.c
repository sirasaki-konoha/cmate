#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char **split_args(const char *input) {
  size_t argc = 0;
  size_t capacity = 10;
  char **argv = malloc(capacity * sizeof(char *));

  const char *p = input;
  while (*p) {
    while (isspace((unsigned char)*p))
      p++;
    if (*p == '\0')
      break;

    const char *start = p;
    while (*p && !isspace((unsigned char)*p))
      p++;

    size_t len = p - start;
    char *token = malloc(len + 1);
    strncpy(token, start, len);
    token[len] = '\0';

    if (argc + 1 >= capacity) {
      capacity *= 2;
      argv = realloc(argv, capacity * sizeof(char *));
    }

    argv[argc++] = token;
  }

  argv[argc] = NULL;
  return argv;
}

void free_args(char **argv) {
  for (size_t i = 0; argv[i] != NULL; i++) {
    free(argv[i]);
  }
  free(argv);
}
