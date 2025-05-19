#ifndef _FILE_IO_H
#define _FILE_IO_H
#include <gen_toml.h>

toml_parsed_t read_and_parse(const char *path);
int create_and_write(const char *path, const char *write);
int create_not_exists(const char *path, const char *write);

#endif /* _FILE_IO_H */
