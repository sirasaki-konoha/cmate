#ifndef _CMATE_ARG_T_H
#define _CMATE_ARG_T_H

typedef struct {
  const char *shortarg;  // Short argument (e.g., "-h")
  const char *longarg;   // Long argument (e.g., "--help")
  const char *takes;     // Argument that takes a value (e.g., "<file>")
  const char *help;      // Help description for the argument
  char *value;           // Value assigned to the argument
  int count;            // Count of how many times the argument was used
} cmate_arg_t;

#endif /* _CMATE_ARG_T_H */
