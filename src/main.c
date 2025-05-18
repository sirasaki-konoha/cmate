#include "file_io.h"
#include <gen_makefile.h>
#include <gen_toml.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
  toml_parsed_t tml = read_and_parse();
  char *p = gen_makefile(tml);
  printf("%s\n", p);
  free(p);
}
