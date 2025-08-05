#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "run/run.h"
#include "utils/term_color.h"
#include "utils/utils.h"
#include "toml/toml_utils.h"
#include "toml/find_toml.h"
#include "utils/run_command_stdout.h"
#include "utils/stb_ds.h"

#ifdef _WIN32
# include <direct.h>
# define chdir(x) _chdir(x)
# define getcwd(x, y) _getcwd(x, y)
#else
# include <unistd.h>
#endif

#define CWD_SIZE 1024

int run_project(const char* toml_file, const char* exec_project) {
	return run_project_with_args(toml_file, exec_project, NULL);
}

int run_project_with_args(const char *toml_file, const char *exec_project, char** args)
{

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

	FILE *file;
	if (! (file = fopen(exec_command, "r"))){
		ERR("The project has not been built yet.\n");
		ret = 1;
		free(exec_command);
		free_toml_parsed(tml, count);
		goto free_and_exit;
	}


	char **args_exec = NULL;

	if (args != NULL) {
		size_t args_len = arrlen(args);
		args_exec = malloc(sizeof(char*) * (args_len + 2)); 
		if (!args_exec) {
			perror("malloc args_exec");
			ret = 1;
			goto free_and_exit;
		}

		args_exec[0] = exec_command;
		for (size_t i = 0; i < args_len; i++) {
			args_exec[i + 1] = args[i];
		}
		args_exec[args_len + 1] = NULL;
	} else {
		char **args_exec_single = malloc(sizeof(char*) * 2);
		if (!args_exec_single) {
			perror("malloc args_exec_single");
			ret = 1;
			goto free_and_exit;
		}
		args_exec_single[0] = exec_command;
		args_exec_single[1] = NULL;
		args_exec = args_exec_single;
	}

	INFO("Running %s\n", exec_command);
	run_command_stdout(exec_command, args_exec);

	free(args_exec);


	free(exec_command);
	free_toml_parsed(tml, count);
	goto free_and_exit;
	
free_and_exit:
	free(toml_dir);
	free(toml_full);
	return ret;
}


