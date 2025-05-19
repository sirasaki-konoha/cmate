#include <file_io.h>
#include <gen_toml.h>
#include <stdio.h>
#include <stdlib.h>
#include <toml.h>

toml_parsed_t read_and_parse(const char *path) {
  toml_parsed_t parsed = {};

  FILE *file = fopen(path, "r");
  if (file == NULL) {
    fprintf(stderr, "Unable to open %s!\n", path);
    exit(EXIT_FAILURE);
  }

  char errbuf[300];
  toml_table_t *conf = toml_parse_file(file, errbuf, sizeof(errbuf));
  fclose(file);

  if (!conf) {
    fprintf(stderr, "TOML parse error: %s\n", errbuf);
    exit(EXIT_FAILURE);
  }

  int count;

  parsed.project_name = get_str(conf, "project");
  parsed.compiler = get_str(conf, "compiler");
  parsed.cflags = get_str(conf, "flags");
  parsed.compile_file = get_arrays(toml_array_in(conf, "compile"), &count);
  count = 0;
  parsed.includes = get_arrays(toml_array_in(conf, "include"), &count);
  count = 0;
  parsed.srcdirs = get_arrays(toml_array_in(conf, "src"), &count);

  count = 0;
  parsed.libraries = get_arrays(toml_array_in(conf, "libs"), &count);

  return parsed;
}
