#include <argtable2.h>
#include <file_io.h>
#include <gen_makefile.h>
#include <gen_toml.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <utils.h>

#ifdef _WIN32
#include <direct.h>
#define MKDIR(dir) _mkdir(dir)
#else
#include <sys/stat.h>
#include <sys/types.h>
#define MKDIR(dir) mkdir(dir, 0755)
#endif

void display_version() {
  printf("gmk 1.0\n");
  printf("Copyright (C) 2025 noa-vliz.\n");
  printf("Licensed under the MIT License\n");
  printf("Source: https://github.com/noa-vliz/gmk\n");
  exit(EXIT_SUCCESS);
}

char *n_strdup(const char *s) {
  char *dup = malloc(strlen(s) + 1);
  if (dup)
    strcpy(dup, s);
  return dup;
}

int main(int argc, char *argv[]) {

  struct arg_lit *help = arg_lit0("h", "help", "Display this help message");
  struct arg_str *output =
      arg_str0("o", "output", "<file>",
               "Change the output file destination (default: Makefile)");
  struct arg_str *toml =
      arg_str0("t", "toml", "<file>",
               "Change the TOML file to parse (default: project.toml)");
  struct arg_lit *version = arg_lit0("v", "version", "Display version info");
  struct arg_lit *init = arg_lit0("i", "init", "Generate a project.toml file");
  struct arg_end *end = arg_end(20);

  void *argtable[] = {help, output, toml, version, init, end};

  if (arg_nullcheck(argtable) != 0) {
    fprintf(stderr, "Failed to build argtable\n");
    return 1;
  }

  int nerrors = arg_parse(argc, argv, argtable);

  if (help->count > 0) {
    printf("Usage: \n");
    arg_print_syntax(stdout, argtable, "\n");
    arg_print_glossary(stdout, argtable, "  %-25s %s\n");
    arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
    return 0;
  }

  if (nerrors > 0) {
    arg_print_errors(stderr, end, argv[0]);
    arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
    return 1;
  }

  if (version->count > 0) {
    display_version();
    arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
    return 0;
  }

  if (init->count > 0) {
    init_project();
  }

  char *output_file = n_strdup("Makefile");
  char *toml_file = n_strdup("project.toml");

  if (output->count > 0)
    output_file = n_strdup(output->sval[0]);

  if (toml->count > 0)
    toml_file = n_strdup(toml->sval[0]);

  toml_parsed_t tml = read_and_parse(toml_file);
  char *p = gen_makefile(tml);
  create_and_write(output_file, p);

  MKDIR("src");
  MKDIR("include");

  free(p);
  free(output_file);
  free(toml_file);

  arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));

  return 0;
}
