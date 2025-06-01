#ifndef _RUN_COMMAND_H
#define _RUN_COMMAND_H

/* impl at run_command.c */
int run_command(const char *cmd, char *const argv[]);

/* impl at run_command_stderr.c */
int run_command_stderr_only(const char *cmd, char *const argv[]);

#endif /* _RUN_COMMAND_H */