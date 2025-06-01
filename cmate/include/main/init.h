#ifndef _INIT_H
#define _INIT_H

int create_source_files();
int create_template_not_exits(const char *dir, const char *filename,
                                     const char *tmpl,
                                     int const template_len);

#endif /* _INIT_H */