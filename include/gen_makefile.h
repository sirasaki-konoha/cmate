#ifndef _GEN_MAKEFILE_H
#define _GEN_MAKEFILE_H
#include <gen_toml.h>

char *gen_makefile(toml_parsed_t *parsed, int count, const char* cmate_version);
char *check_depends(toml_parsed_t *parsed);
char *auto_detect_compiler();

#endif
