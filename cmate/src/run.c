#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tomlc99/toml.h"

#include "run.h"
#include "term_color.h"
#include "utils.h"
#include "toml_utils.h"
#include "find_toml.h"
#include "run_command_stdout.h"

#ifdef _WIN32
# include <direct.h>
# define chdir(x) _chdir(x)
# define getcwd(x, y) _getcwd(x, y)
#else
# include <unistd.h>
#endif

#define CWD_SIZE 1024

int run_project(const char* toml_file, const char* exec_project) {
	char *toml_dir = get_toml_dir(toml_file);
	char *toml_full = get_toml_file(toml_file);
	char old_dir[CWD_SIZE];
	int ret = 0;

	if (getcwd(old_dir, sizeof(old_dir)) == NULL){
		perror("Failed to get current_dir");
		ret = 1;
		goto free_and_exit;
	}

	if (toml_dir == NULL) {
		ERR("Cmate.toml not found\n");
		ret = 1;
		goto free_and_exit;
	}

	int count = 0;
	toml_parsed_t *tml = parse_toml(toml_full, &count);
	if (count == 0) {
		ERR("%s is not configured!\n", toml_file);
		ret = 1;
		free_toml_parsed(tml, count);
		goto free_and_exit;
	}

	int found = 0;
	for (int i = 0; i < count; i++) {
    		if (strcmp(tml[i].project_name, exec_project) == 0) {
			found = 1;
			break;
		}
	}


	if (found != 1){
		ERR("Project not found: %s", exec_project);
		ret = 1;
		free_toml_parsed(tml, count);
		goto free_and_exit;
	}

#ifdef _WIN32
	char *exec_command = format_string("%s\\bin\\%s", toml_dir, exec_project);
#else
	char *exec_command = format_string("%s/bin/%s", toml_dir, exec_project);
#endif
	char *args[] = {exec_command, NULL};
	INFO("Running %s\n", exec_command);
	run_command_stdout(exec_command, args);
		

	free(exec_command);
	free_toml_parsed(tml, count);
	goto free_and_exit;
	
free_and_exit:
	free(toml_dir);
	free(toml_full);
	return ret;
}


