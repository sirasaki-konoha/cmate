#include <file_io.h>
#include <stdio.h>
#include <stdlib.h>

void create_and_write(const char *path, const char *write) {
  FILE *fp = fopen(path, "w+");

  if (fp == NULL) {
    fprintf(stderr, "Unable to write %s\n", path);
    exit(EXIT_FAILURE);
  }

  fprintf(fp, "%s\n", write);
  fclose(fp);
}
