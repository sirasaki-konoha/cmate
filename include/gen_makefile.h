#ifndef _GEN_MAKEFILE_H
#define _GEN_MAKEFILE_H
#include <gen_toml.h>

char *gen_makefile(toml_parsed_t parsed);
int check_depends(toml_parsed_t parsed);

#endif
