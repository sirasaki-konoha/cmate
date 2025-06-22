#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

#ifdef _WIN32
# include <direct.h>
#else
# include <unistd.h>
#endif

#include "build.h"
#include "term_color.h"
#include "toml_utils.h"
#include "utils.h"
#include "file_io.h"
#include "run_command.h"
#include "main/process_makefile.h"

#ifdef _WIN32
# define chdir(x) _chdir(x)
#endif

#define CWD_SIZE 1024

char* get_toml_dir(const char* toml_file) {
    int dirs = 0;
    char old_dir[CWD_SIZE];
    if (getcwd(old_dir, sizeof(old_dir)) == NULL) {
        perror("failed to get current dir");
        return NULL;
    }

    char *cwd = calloc(CWD_SIZE, sizeof(char));
    if (!cwd) return NULL;

    while (1) {
        FILE *f = fopen(toml_file, "r");
        if (f != NULL) {
            fclose(f);
            if (getcwd(cwd, CWD_SIZE) != NULL) {
                if (chdir(old_dir) != 0)
                        perror("failed to return old dir");
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

    char *cwd = calloc(CWD_SIZE, sizeof(char));
    if (!cwd) return NULL;

    while (1) {
        FILE *f = fopen(toml_file, "r");
        if (f != NULL) {
            fclose(f);
            if (getcwd(cwd, CWD_SIZE) != NULL) {
                #ifdef _WIN32
                    cwd = format_string("%s\\%s", cwd, toml_file);
                #else
                    cwd = format_string("%s/%s", cwd, toml_file);
                #endif
                if (chdir(old_dir) != 0)
                        perror("failed to return old dir");
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

