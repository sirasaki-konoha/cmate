#include "main/main.h"

#include <stdio.h>

#include "utils/term_color.h"

cmate_arg_t **args = NULL;
char **run_args = NULL;

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

int argparse_with_run_args(int argc, char **argv, cmate_arg_t **args) {
  int i;
  int dash_dash_found = 0;
  int dash_dash_index = -1;

  int has_run_option = 0;
  for (i = 1; i < argc; i++) {
    if (strcmp(argv[i], "--") == 0) {
      dash_dash_found = 1;
      dash_dash_index = i;
      break;
    }
    if (strcmp(argv[i], "-r") == 0 || strcmp(argv[i], "--run") == 0) {
      has_run_option = 1;
    }
  }

  if (dash_dash_found && !has_run_option) {
    ERR("-- can only be used with -r/--run option\n");
    return 1;
  }

  if (dash_dash_found && has_run_option) {
    for (i = dash_dash_index + 1; i < argc; i++) {
      arrput(run_args, argv[i]);
    }

    argc = dash_dash_index;
  }

  return argparse(argc, argv, args);
}

int main(int argc, char **argv) {
  enable_ansi_escape_codes();

  cmate_arg_t output = {"o",      "output",
                        "<file>", "Specify the output file (default: Makefile)",
                        NULL,     0};
  cmate_arg_t toml = {
      "t",      "toml",
      "<file>", "Specify the TOML file to parse (default: project.toml)",
      NULL,     0};
  cmate_arg_t run = {
      "r",
      "run",
      "<project>",
      "Run the specified project (use -- to pass arguments to the program)",
      NULL,
      0};
  cmate_arg_t help = {"h", "help", NULL, "Display this help message", NULL, 0};
  cmate_arg_t version = {"V",  "version", NULL, "Display version information",
                         NULL, 0};
  cmate_arg_t init = {"i",  "init", NULL, "Generate a project.toml file",
                      NULL, 0};
  cmate_arg_t build = {"b", "build", NULL, "Build the project", NULL, 0};
  cmate_arg_t clean = {"c", "clean", NULL, "Clean the project", NULL, 0};

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
  arrput(args, &clean);
  arrput(args, &run);

  nerrors = argparse_with_run_args(argc, argv, args);

  if (help.count > 0) {
    display_help(args);
    goto cleanup;
  }

  if (nerrors > 0) {
    exit_code = EXIT_FAILURE;
    goto cleanup;
  }

  if (version.count > 0) {
    display_version();
    goto cleanup;
  }

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

  output_file =
      safe_strdup(output.count > 0 ? output.value : DEFAULT_OUTPUT_FILE);
  toml_file = safe_strdup(toml.count > 0 ? toml.value : DEFAULT_TOML_FILE);

  if (run.count > 0) {
    run_project_with_args(toml_file, run.value, run_args);
    goto cleanup;
  }

  if (clean.count > 0) {
    clean_project(toml_file);
    if (build.count > 0) {
      if (build_project(toml_file, 1) == 0) {
        if (run.count > 0) {
          run_project_with_args(toml_file, run.value, run_args);
        }
      }
    }
    goto cleanup;
  }

  if (build.count > 0) {
    if (build_project(toml_file, 1) == 0) {
      if (run.count > 0) {
        run_project_with_args(toml_file, run.value, run_args);
      }
    }
    goto cleanup;
  }

  if (!output_file || !toml_file) {
    ERR("Memory allocation failed\n");
    exit_code = EXIT_FAILURE;
    goto cleanup;
  }

  char *toml_dir = get_toml_dir(toml_file);
  char *toml_full = get_toml_file(toml_file);
  char old_dir[1024];

  if (getcwd(old_dir, sizeof(old_dir)) == NULL) {
    perror("Failed to get old dir");
    goto cleanup;
  }

  if (toml_dir == NULL) {
    ERR("Cmate.toml not found\n");
    goto cleanup;
  }

  if (chdir(toml_dir) != 0) {
    perror("Failed to move toml_dir!");
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
  free(output_file);
  free(toml_file);
  if (run_args) {
    arrfree(run_args);
  }
  return exit_code;
}
