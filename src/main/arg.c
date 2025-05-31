#include "main/main.h"
#include "main/arg.h"

/**
 * Parse arguments from command line
 * @param argc Argument count
 * @param argv Argument vector
 * @return Number of errors encountered during parsing
 *         0 on success, non-zero on failure
 */
int argparse(int argc, char **argv, cmate_arg_t **args) {
  int i, j;
  int r = 0;
  for (i = 1; i < argc; i++) {
    int m = 0;
    for (j = 0; j < arrlen(args); j++) {
      int s = strlen(argv[i]) > 1
                  ? ((argv[i][0] == '-' &&
                      strcmp(argv[i] + 1, args[j]->shortarg) == 0)
                         ? 1
                         : 0)
                  : 0;
      int l = strlen(argv[i]) > 2
                  ? ((argv[i][0] == '-' && argv[i][1] == '-' &&
                      strcmp(argv[i] + 2, args[j]->longarg) == 0)
                         ? 1
                         : 0)
                  : 0;
      if (s || l) {
        m = 1;
        args[j]->count++;
        if (args[j]->takes != NULL) {
          args[j]->value = argv[++i];
          if (args[j]->value == NULL) {
            r++;
            ERROR("%s: missing argument: %s\n", argv[0], argv[i - 1]);
          }
        }
        break;
      }
    }
    if (!m) {
      r++;
      ERROR("%s: invalid flag: %s\n", argv[0], argv[i]);
    }
  }
  return r;
}

/**
 * Display help message
 * @param argtable Argument table
 */
void display_help(cmate_arg_t **args) {
  int i;
  printf("Usage: \n");
  for (i = 0; i < arrlen(args); i++) {
    char buf[512];
    sprintf(buf, "-%s --%s", args[i]->shortarg, args[i]->longarg);
    if (args[i]->takes != NULL)
      sprintf(buf + strlen(buf), " %s", args[i]->takes);
    printf("  %-25s %s\n", buf, args[i]->help);
  }
}