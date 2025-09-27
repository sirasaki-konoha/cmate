#ifndef _RUN_H
#define _RUN_H

int run_project(const char* toml_file, const char* project_name);
int run_project_with_args(const char* toml_file, const char* project_name,
                          char** args);

#endif /* _RUN_H */
