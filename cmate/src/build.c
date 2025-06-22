#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

#ifdef _WIN32
# include <direct.h>
#else
# include <unistd.h>
#endif

#include "build.h"
#include "find_toml.h"
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


int build_project(const char* toml_file) {
    char *toml_full_path = get_toml_file(toml_file);
    char *toml_dir = get_toml_dir(toml_file);
    char old_dir[CWD_SIZE];
    int ret = 0;

    if (getcwd(old_dir, sizeof(old_dir)) == NULL){
	    perror("Failed to get current dir");
	    return 1;
    }

    if (chdir(toml_dir) != 0){ 
	    perror("Unable to find Cmate.toml!");
	    return 1;
    }

    INFO("Using configuration from: %s\n", toml_full_path);
    
    process_makefile(toml_full_path, "Makefile", 0);

    INFO("Cleaning up previous build artifacts\n");
    char *args_clean[] = {"make", "clean", NULL};
    if (run_command_stderr_only("make", args_clean))
        ERR("Failed to clean previous build artifacts. Continuing with build\n");
    
    INFO("Running make (make -j)\n");
    char *args[] = {"make", "-j", NULL};
    if (!run_command_stderr_only("make", args)) {
        INFO("Build completed successfully.\n");
	goto cleanup;
    } else {
        ERR("Build failed. Please check the output for errors.\n");
        ERR("You can try to run 'make' manually to see more details.\n");
        ERR("If the issue persists, please check your Cmate.toml configuration.\n");

        INFO("Cleaning up\n");
        char *args_clean[] = {"make", "clean", NULL};
        if (run_command_stderr_only("make", args_clean))
            ERR("Failed to clean up after build failure.\n");

	ret = 1;
	goto cleanup;
    }

cleanup:
    free(toml_full_path);
    free(toml_dir);
    if (chdir(old_dir) != 0) {
	    perror("Failed to return old dir");
    }
    return ret;

}

