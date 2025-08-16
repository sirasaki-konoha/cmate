#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
# include <direct.h>
# include <windows.h>
#else
# include <unistd.h>
#endif /* _WIN32 */

#include "clean/clean.h"
#include "toml/find_toml.h"
#include "utils/run_command.h"
#include "utils/utils.h"
#include "utils/term_color.h"

#ifdef _WIN32
# define chdir(x) _chdir(x)
# define getcwd(x, y) _getcwd(x, y)
#endif

#define CWD_SIZE 1024

int clean_project(const char* toml_file) {
	char *toml_full_path = get_toml_file(toml_file);
	char *toml_dir = get_toml_dir(toml_file);
	char old_dir[CWD_SIZE];

	if (getcwd(old_dir, sizeof(old_dir)) == NULL) {
		perror("Failed to get current dir");
    free(toml_full_path);
    free(toml_dir);
    return 1;
	}

	if (toml_dir == NULL) {
		ERR("Cmate.toml not found\n");
		free(toml_full_path);
		free(toml_dir);
		return 1;
	}

	if (chdir(toml_dir) != 0) {
		perror("Failed to chenge working directory!");
		free(toml_full_path);
		free(toml_dir);
		return 1;
	}

	FILE *file;
	if (!(file = fopen("Makefile", "r"))) {
		ERR("%s/Makefile not found\n", toml_dir);
		free(toml_full_path);
		free(toml_dir);
		return 1;
	}

	char *args[] = {"make", "clean", NULL};

	INFO("Cleaning project...\n");
	if (run_command_stderr_only("make", args)) {
		ERR("Clean failed\n");
		free(toml_full_path);
		free(toml_dir);
		return 1;
	}

	return 0;
}

