#include "main/main.h"
#include "term_color.h"
#include <stdio.h>

cmate_arg_t **args = NULL;

/**
 * Display version information
 */
static void display_version(void) {
#ifdef NDEBUG
  printf("cmate %s\n", CMATE_VERSION);
#else
  printf("cmate %s (debug build)\n", CMATE_VERSION);
  printf("Compiled on: %s, %s\n", __DATE__, __TIME__);
#endif

  printf("%s\n", CMATE_COPYRIGHT);
  printf("%s\n", CMATE_LICENSE);
  printf("%s\n", CMATE_SOURCE);
}

int main(int argc, char **argv) {
  enable_ansi_escape_codes();

  // Define command line arguments
  cmate_arg_t help = {"h", "help", NULL, "Display this help message", NULL, 0};
  cmate_arg_t output = {
      "o",      "output",
      "<file>", "Change the output file destination (default: Makefile)",
      NULL,     0};
  cmate_arg_t toml = {
      "t",      "toml",
      "<file>", "Change the TOML file to parse (default: project.toml)",
      NULL,     0};
  cmate_arg_t version = {"V", "version", NULL, "Display version info", NULL, 0};
  cmate_arg_t init = {"i",  "init", NULL, "Generate a project.toml file",
                      NULL, 0};
  cmate_arg_t build = {"b", "build", NULL, "Build the project", NULL, 0};

  int exit_code = EXIT_SUCCESS;
  char *output_file = NULL;
  char *toml_file = NULL;
  int nerrors = 0;

  arrput(args, &help);
  arrput(args, &output);
  arrput(args, &toml);
  arrput(args, &version);
  arrput(args, &init);
  arrput(args, &build);

  nerrors = argparse(argc, argv, args);

  // Display help message
  if (help.count > 0) {
    display_help(args);
    goto cleanup;
  }

  // Display error messages
  if (nerrors > 0) {
    exit_code = EXIT_FAILURE;
    goto cleanup;
  }

  // Display version information
  if (version.count > 0) {
    display_version();
    goto cleanup;
  }

  // Initialize project
  if (init.count > 0) {
    if (init_project() != 0) {
      ERR("Failed to initialize project\n");
      exit_code = EXIT_FAILURE;
      goto cleanup;
    }

    if (create_source_files() != 0) {
      exit_code = EXIT_FAILURE;
      goto cleanup;
    }

    goto cleanup;
  }

  // Set file paths
  output_file =
      safe_strdup(output.count > 0 ? output.value : DEFAULT_OUTPUT_FILE);
  toml_file = safe_strdup(toml.count > 0 ? toml.value : DEFAULT_TOML_FILE);

  // Build the project
  if (build.count > 0) {
    build_project(toml_file);
    goto cleanup;
  }

  if (!output_file || !toml_file) {
    ERR("Memory allocation failed\n");
    exit_code = EXIT_FAILURE;
    goto cleanup;
  }


  // Process Makefile
  char *toml_dir = get_toml_dir(toml_file);
  char *toml_full = get_toml_file(toml_file);
  char old_dir[1024];
  if (getcwd(old_dir, sizeof(old_dir)) == NULL) {
	  perror("Failed to get old dir");
	  goto cleanup;
  }

  if (chdir(toml_dir) != 0) {
    perror("Failed to find toml file");
    goto cleanup;
  }

  INFO("Cmate.toml found in %s\n", toml_dir);
  if (process_makefile(toml_full, output_file, 1) != 0) {
    exit_code = EXIT_FAILURE;
    free(toml_dir);
    free(toml_full);
    if (chdir(old_dir) != 0) {
	    perror("Failed to find toml file");
	    goto cleanup;
    }
    goto cleanup;
  }
  if (chdir(old_dir) != 0) {
	    perror("Failed to find toml file");
	    goto cleanup;
  }
  free(toml_dir);
  free(toml_full);

cleanup:
  // Release resources
  free(output_file);
  free(toml_file);

  return exit_code;
}
