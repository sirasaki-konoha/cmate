#ifndef _GEN_MAKEFILE_H
#define _GEN_MAKEFILE_H
#include "toml/toml_utils.h"

char *gen_makefile(toml_parsed_t *parsed, int count, const char* cmate_version, int output);
int get_array_len(char **array);

#endif
