#ifndef _ARG_H
#define _ARG_H
#include "main/cmate_arg_t.h"

int argparse(int argc, char **argv, cmate_arg_t **args);
void display_help(cmate_arg_t **args);

#endif /* _ARG_H */
