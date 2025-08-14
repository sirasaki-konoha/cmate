#include <stdio.h>
#include <stdlib.h>

#include "file_io/file_io.h"

int create_and_write(const char *path, const char *write) {
  FILE *fp = fopen(path, "w+");

  if (fp == NULL) {
    return 1;
  }

  fprintf(fp, "%s\n", write);
  fclose(fp);
  return 0;
}

#ifdef _WIN32
#include <io.h>
#define ACCESS(path, mode) _access(path, mode)
#else
#include <unistd.h>
#define ACCESS(path, mode) access(path, mode)
#endif

int create_not_exists(const char *path, const char *write) {
  if (ACCESS(path, R_OK) != 0) {
    create_and_write(path, write);
  } else {
    return 1;
  }
  return 0;
}
