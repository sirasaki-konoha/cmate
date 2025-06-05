#ifndef _FILE_IO_H
#define _FILE_IO_H
#include "toml_utils.h"

/* define at file_io/write.c */
int create_and_write(const char *path, const char *write);
int create_not_exists(const char *path, const char *write);
int create_template_not_exits(const char *dir, const char *filename,
                              const char *template, int const template_len);

#endif /* _FILE_IO_H */
