#include <stdio.h>

#include "build.h"
#include "term_color.h"
#include "utils.h"
#include "toml_utils.h"
#include "file_io.h"
#include "main/process_makefile.h"


int build_project(const char* toml_file) {
    INFO("Building project...\n");
    INFO("Using configuration from: %s\n", toml_file);
    INFO("Regenerating Makefile...\n");
    process_makefile(toml_file, "Makefile", 0);
    
    INFO("Running make...\n");
    char *args[] = {"-j", NULL};
    if (!run_command("make", args)) {
        INFO("Build completed successfully.\n");
        INFO("To run the project, use: ./bin/<project_name>\n");
        return 0;
    } else {
        ERROR("Build failed. Please check the output for errors.\n");
        INFO("You can try to run 'make' manually to see more details.\n");
        INFO("Cleaning up...\n");

        char *args[] = {"clean", NULL};
        if (run_command("make", args))
            ERROR("Failed to clean up after build failure.\n");

        INFO("If the issue persists, please check your Cmate.toml configuration.\n");
        ERROR("%s: Build failed.\n", RED("FALURE"));
        return 1;
    }


}

