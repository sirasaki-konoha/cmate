#include "utils.h"
#include <gen_makefile.h>
#include <gen_toml.h>
#include <stdio.h>
#include <stdlib.h>
#include <toml.h>
#include <utils.h>

int check_depends(toml_parsed_t parsed) {
  printf("[INFO] Checking dependencies\n");

  // 1. コマンド文字列を構築
  int len = snprintf(NULL, 0, "%s --version", parsed.compiler);
  char *compiler = malloc(len + 1);
  if (compiler == NULL) {
    perror("malloc failed");
    exit(EXIT_FAILURE);
  }
  snprintf(compiler, len + 1, "%s --version", parsed.compiler);

  // 2. 引数を分割 (例: "gcc --version" → ["gcc", "--version", NULL])
  char **args = split_args(compiler);
  if (args == NULL || args[0] == NULL) {
    printf("=> Failed to parse command\n");
    free(compiler);
    return 1;
  }

  // 3. コマンド実行
  int exit_code = run_command(args[0], args);

  // 4. メモリ解放
  free(compiler);
  free_args(args);

  // 5. 結果判定
  if (exit_code != 0) {
    printf("[ERROR] The C compiler '%s' is not set up!\n", parsed.compiler);
    return 1;
  }

  printf("[INFO] The C Compiler is %s\n", parsed.compiler);
  return 0;
}
