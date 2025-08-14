#ifndef _DEPEND_H
#define _DEPEND_H
#include "toml/toml_utils.h"

char *auto_detect_compiler();
char *check_depends(toml_parsed_t *parsed);

#endif /* _DEPEND_H */
