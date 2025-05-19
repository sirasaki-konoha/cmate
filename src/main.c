#include <argtable2.h>
#include <embed_mkfile.h>
#include <errno.h>
#include <file_io.h>
#include <gen_makefile.h>
#include <gen_toml.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <utils.h>

#ifdef _WIN32
#include <direct.h>
#define MKDIR(dir) _mkdir(dir)
#else
#include <sys/stat.h>
#include <sys/types.h>
#define MKDIR(dir) mkdir(dir, 0755)
#endif

#define GMK_VERSION "1.0"
#define GMK_COPYRIGHT "Copyright (C) 2025 noa-vliz."
#define GMK_LICENSE "Licensed under the MIT License"
#define GMK_SOURCE "Source: https://github.com/noa-vliz/gmk"

#define DEFAULT_OUTPUT_FILE "Makefile"
#define DEFAULT_TOML_FILE "project.toml"
#define SRC_DIRECTORY "src"
#define INCLUDE_DIR "include"
#define MAIN_C_FILE "main.c"

/**
 * Display version information
 */
static void display_version(void) {
  printf("gmk %s\n", GMK_VERSION);
  printf("%s\n", GMK_COPYRIGHT);
  printf("%s\n", GMK_LICENSE);
  printf("%s\n", GMK_SOURCE);
}

/**
 * Display help message
 * @param argtable Argument table
 */
static void display_help(void **argtable) {
  printf("Usage: \n");
  arg_print_syntax(stdout, argtable, "\n");
  arg_print_glossary(stdout, argtable, "  %-25s %s\n");
}

/**
 * Create source directory and main.c file
 * @return 0 on success, non-zero on failure
 */
static int create_source_files(void) {
  int result = 0;

  // Create source directory
  if (MKDIR(SRC_DIRECTORY) != 0) {
    // Not an error if directory already exists
    if (errno != EEXIST) {
      fprintf(stderr, "Failed to create source directory\n");
      return 1;
    }
  }

  // Create include directory
  if (MKDIR(INCLUDE_DIR) != 0) {
    // Not an error if directory already exists
    if (errno != EEXIST) {
      fprintf(stderr, "Failed to create include directory\n");
      return 1;
    }
  }

  // Allocate memory for main.c template
  char *main_template = malloc(template_main_template_len + 1);
  if (!main_template) {
    fprintf(stderr, "Memory allocation failed for template\n");
    return 1;
  }

  // Copy template and add null terminator
  memcpy(main_template, template_main_template, template_main_template_len);
  main_template[template_main_template_len] = '\0';

  // Build file path
  char filepath[256];
#ifdef _WIN32
  snprintf(filepath, sizeof(filepath), "%s\\%s", SRC_DIRECTORY, MAIN_C_FILE);
#else
  snprintf(filepath, sizeof(filepath), "%s/%s", SRC_DIRECTORY, MAIN_C_FILE);
#endif

  // Create file and write content
  if (create_and_write(filepath, main_template) != 0) {
    fprintf(stderr, "Failed to create main.c template\n");
    result = 1;
  }

  free(main_template);
  return result;
}

/**
 * Process TOML file and generate Makefile
 * @param toml_file Path to TOML file
 * @param output_file Path to output file
 * @return 0 on success, non-zero on failure
 */
static int process_makefile(const char *toml_file, const char *output_file) {
  if (!toml_file || !output_file) {
    fprintf(stderr, "Invalid file paths\n");
    return 1;
  }

  // Parse TOML file
  toml_parsed_t tml = read_and_parse(toml_file);

  // Generate Makefile content
  if (check_depends(tml) != 0) {
    return 1;
  }
  char *makefile_content = gen_makefile(tml);
  if (!makefile_content) {
    fprintf(stderr, "Failed to generate Makefile content\n");
    free_toml_parsed(tml);
    return 1;
  }

  // Create and write Makefile
  if (create_and_write(output_file, makefile_content) != 0) {
    fprintf(stderr, "Failed to write to output file: %s\n", output_file);
    free(makefile_content);
    free_toml_parsed(tml);
    return 1;
  }

  free(makefile_content);
  free_toml_parsed(tml);

  return 0;
}

int main(int argc, char *argv[]) {
  // Define command line arguments
  struct arg_lit *help = arg_lit0("h", "help", "Display this help message");
  struct arg_str *output =
      arg_str0("o", "output", "<file>",
               "Change the output file destination (default: Makefile)");
  struct arg_str *toml =
      arg_str0("t", "toml", "<file>",
               "Change the TOML file to parse (default: project.toml)");
  struct arg_lit *version = arg_lit0("v", "version", "Display version info");
  struct arg_lit *init = arg_lit0("i", "init", "Generate a project.toml file");
  struct arg_end *end = arg_end(20);

  void *argtable[] = {help, output, toml, version, init, end};
  int exit_code = EXIT_SUCCESS;
  char *output_file = NULL;
  char *toml_file = NULL;

  // Validate argtable
  if (arg_nullcheck(argtable) != 0) {
    fprintf(stderr, "Failed to build argtable\n");
    return EXIT_FAILURE;
  }

  // Parse arguments
  int nerrors = arg_parse(argc, argv, argtable);

  // Display help message
  if (help->count > 0) {
    display_help(argtable);
    goto cleanup;
  }

  // Display error messages
  if (nerrors > 0) {
    arg_print_errors(stderr, end, argv[0]);
    exit_code = EXIT_FAILURE;
    goto cleanup;
  }

  // Display version information
  if (version->count > 0) {
    display_version();
    goto cleanup;
  }

  // Initialize project
  if (init->count > 0) {
    if (init_project() != 0) {
      fprintf(stderr, "Failed to initialize project\n");
      exit_code = EXIT_FAILURE;
      goto cleanup;
    }
    goto cleanup;
  }

  // Set file paths
  output_file =
      safe_strdup(output->count > 0 ? output->sval[0] : DEFAULT_OUTPUT_FILE);
  toml_file = safe_strdup(toml->count > 0 ? toml->sval[0] : DEFAULT_TOML_FILE);

  if (!output_file || !toml_file) {
    fprintf(stderr, "Memory allocation failed\n");
    exit_code = EXIT_FAILURE;
    goto cleanup;
  }

  // Process Makefile
  if (process_makefile(toml_file, output_file) != 0) {
    exit_code = EXIT_FAILURE;
    goto cleanup;
  }

  // Create source files
  if (create_source_files() != 0) {
    exit_code = EXIT_FAILURE;
    goto cleanup;
  }

cleanup:
  // Release resources
  free(output_file);
  free(toml_file);
  arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));

  return exit_code;
}
