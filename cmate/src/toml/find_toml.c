#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
#include <direct.h>
#else
#include <unistd.h>
#endif

#include "build/build.h"
#include "file_io/file_io.h"
#include "main/process_makefile.h"
#include "toml/toml_utils.h"
#include "utils/run_command.h"
#include "utils/term_color.h"
#include "utils/utils.h"

#ifdef _WIN32
#define chdir(x) _chdir(x)
#define getcwd(x, y) _getcwd(x, y)
#endif

#define CWD_SIZE 1024

char* get_toml_dir(const char* toml_file) {
  int dirs = 0;
  char old_dir[CWD_SIZE];
  if (getcwd(old_dir, sizeof(old_dir)) == NULL) {
    perror("failed to get current dir");
    return NULL;
  }

  char* cwd = calloc(CWD_SIZE, sizeof(char));
  if (!cwd) return NULL;

  while (1) {
    FILE* f = fopen(toml_file, "r");
    if (f != NULL) {
      fclose(f);
      if (getcwd(cwd, CWD_SIZE) != NULL) {
        if (chdir(old_dir) != 0) perror("failed to return old dir");
        return cwd;
      } else {
        perror("getcwd");
        free(cwd);
        return NULL;
      }
    } else if (errno == ENOENT) {
      if (dirs == 3) {
        free(cwd);
        return NULL;
      }
      if (chdir("..") != 0) {
        perror("chdir failed");
        free(cwd);
        return NULL;
      }
      dirs++;
      continue;
    } else {
      perror("fopen");
      free(cwd);
      return NULL;
    }
  }
}

char* get_toml_file(const char* toml_file) {
  int dirs = 0;
  char old_dir[CWD_SIZE];
  if (getcwd(old_dir, sizeof(old_dir)) == NULL) {
    perror("failed to get current dir");
    return NULL;
  }

  char* cwd = calloc(CWD_SIZE, sizeof(char));
  if (!cwd) return NULL;

  while (1) {
    FILE* f = fopen(toml_file, "r");
    if (f != NULL) {
      fclose(f);
      if (getcwd(cwd, CWD_SIZE) != NULL) {
#ifdef _WIN32
        cwd = format_string("%s\\%s", cwd, toml_file);
#else
        cwd = format_string("%s/%s", cwd, toml_file);
#endif
        if (chdir(old_dir) != 0) perror("failed to return old dir");
        return cwd;
      } else {
        perror("getcwd");
        free(cwd);
        return NULL;
      }
    } else if (errno == ENOENT) {
      if (dirs == 3) {
        free(cwd);
        return NULL;
      }
      if (chdir("..") != 0) {
        perror("chdir failed");
        free(cwd);
        return NULL;
      }
      dirs++;
      continue;
    } else {
      perror("fopen");
      free(cwd);
      return NULL;
    }
  }
}
