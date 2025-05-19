#include <file_io.h>
#include <stdio.h>
#include <stdlib.h>

int create_and_write(const char *path, const char *write) {
  FILE *fp = fopen(path, "w+");

  if (fp == NULL) {
    return 1;
  }

  fprintf(fp, "%s\n", write);
  fclose(fp);
  return 0;
}
