#include <stdio.h>

#include "build.h"
#include "term_color.h"
#include "toml_utils.h"
#include "file_io.h"
#include "run_command.h"
#include "main/process_makefile.h"


int build_project(const char* toml_file) {
    INFO("Building project...\n");
    INFO("Using configuration from: %s\n", toml_file);
    INFO("Regenerating Makefile...\n");
    process_makefile(toml_file, "Makefile", 0);

    INFO("Cleaning up previous build artifacts...\n");
    char *args_clean[] = {"make", "clean", NULL};
    if (run_command_stderr_only("make", args_clean))
        ERR("Failed to clean previous build artifacts. Continuing with build...\n");
    
    INFO("Running make...\n");
    char *args[] = {"make", "-j", NULL};
    if (!run_command_stderr_only("make", args)) {
        INFO("Build completed successfully.\n");
        INFO("To run the project, use: ./bin/<project_name>\n");
        return 0;
    } else {
        ERR("Build failed. Please check the output for errors.\n");
        ERR("You can try to run 'make' manually to see more details.\n");
        ERR("If the issue persists, please check your Cmate.toml configuration.\n");

        INFO("Cleaning up...\n");
        char *args_clean[] = {"make", "clean", NULL};
        if (run_command_stderr_only("make", args_clean))
            ERR("Failed to clean up after build failure.\n");

        return 1;
    }


}

