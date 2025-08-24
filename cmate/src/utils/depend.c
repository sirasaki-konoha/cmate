#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "makefile/gen_makefile.h"
#include "toml/toml_utils.h"
#include "tomlc99/toml.h"
#include "utils/run_command.h"
#include "utils/term_color.h"
#include "utils/utils.h"

char *auto_detect_compiler() {
  char *compilers[] = {"gcc", "clang", "cc", "zig cc", "tcc", NULL};
  int len = get_array_len(compilers);
  char *detect_compiler = NULL;

  for (int i = 0; i < len; i++) {
    char *current_compiler = compilers[i];
    int len = snprintf(NULL, 0, "%s --version", current_compiler);
    char *compiler = malloc(len + 1);
    if (compiler == NULL) {
      perror("malloc failed");
      exit(EXIT_FAILURE);
    }
    snprintf(compiler, len + 1, "%s --version", current_compiler);

    char **args = split_args(compiler);
    if (args == NULL || args[0] == NULL) {
      printf("=> Failed to parse command\n");
      free(compiler);
      continue;
    }

    int exit_code = run_command(args[0], args);
    free_args(args);
    free(compiler);

    if (exit_code != 0) {
      continue;
    }

    detect_compiler = current_compiler;
    break;
  }

  return detect_compiler;
}

char *check_depends(toml_parsed_t *parsed) {
  INFO("Checking dependencies\n");

  if (parsed->compiler == NULL) {
    printf("!!! Compiler is not specified in configuration\n");
    return NULL;
  }

  int len = snprintf(NULL, 0, "%s --version", parsed->compiler);
  char *compiler = malloc(len + 1);
  if (compiler == NULL) {
    perror("malloc failed");
    exit(EXIT_FAILURE);
  }
  snprintf(compiler, len + 1, "%s --version", parsed->compiler);

  char **args = split_args(compiler);
  if (args == NULL || args[0] == NULL) {
    printf("=> Failed to parse command\n");
    free(compiler);
    return NULL;
  }

  int exit_code = run_command(args[0], args);

  free(compiler);
  free_args(args);

  if (exit_code != 0) {
    ERR("The C compiler '%s' is not set up!\n", parsed->compiler);
    return NULL;
  }

  return parsed->compiler;
}
