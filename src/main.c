#include "term_color.h"
#include <embed_mkfile.h>
#include <errno.h>
#include <file_io.h>
#include <gen_makefile.h>
#include <gen_toml.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <utils.h>

#include <stb_ds.h>

#ifdef _WIN32
#include <direct.h>
#define MKDIR(dir) _mkdir(dir)
#else
#include <sys/stat.h>
#include <sys/types.h>
#define MKDIR(dir) mkdir(dir, 0755)
#endif

#define GMK_VERSION "1.1"
#define GMK_COPYRIGHT "Copyright (C) 2025 noa-vliz, rock-db."
#define GMK_LICENSE "Licensed under the MIT License"
#define GMK_SOURCE "Source: https://github.com/rock-db/cate"

#define DEFAULT_OUTPUT_FILE "Makefile"
#define DEFAULT_TOML_FILE "project.toml"
#define SRC_DIRECTORY "src"
#define INCLUDE_DIR "include"
#define MAIN_C_FILE "main.c"

typedef struct {
	const char* shortarg;
	const char* longarg;
	const char* takes;
	const char* help;
	char* value;
	int count;
} gmk_arg_t;
gmk_arg_t **args = NULL;

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
static void display_help(void) {
  int i;
  printf("Usage: \n");
  for(i = 0; i < arrlen(args); i++){
    char buf[512];
    sprintf(buf, "-%s --%s", args[i]->shortarg, args[i]->longarg);
    if(args[i]->takes != NULL) sprintf(buf + strlen(buf), " %s", args[i]->takes);
    printf("  %-25s %s\n", buf, args[i]->help);
  }
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
      ERROR("Failed to create source directory\n");
      return 1;
    }
  }

  // Create include directory
  if (MKDIR(INCLUDE_DIR) != 0) {
    // Not an error if directory already exists
    if (errno != EEXIST) {
      ERROR("Failed to create include directory\n");
      return 1;
    }
  }

  // Allocate memory for main.c template
  char *main_template = malloc(template_main_template_len + 1);
  if (!main_template) {
    ERROR("Memory allocation failed for template\n");
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
  create_not_exists(filepath, main_template);
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
    ERROR("Invalid file paths\n");
    return 1;
  }

  int count = 0;
  // Parse TOML file
  toml_parsed_t *tml = read_and_parse(toml_file, &count);

  // Generate Makefile content
  char *makefile_content = gen_makefile(tml, count);

  if (!makefile_content) {
    ERROR("Failed to generate Makefile content\n");
    free_toml_parsed(tml, count);
    return 1;
  }

  // Create and write Makefile
  if (create_and_write(output_file, makefile_content) != 0) {
    ERROR("Failed to write to output file: %s\n", output_file);
    free(makefile_content);
    free_toml_parsed(tml, count);
    return 1;
  }

  free(makefile_content);

  return 0;
}

static int argparse(int argc, char **argv){
  int i, j;
  int r = 0;
  for(i = 1; i < argc; i++){
    int m = 0;
    for(j = 0; j < arrlen(args); j++){
      int s = strlen(argv[i]) > 1 ? ((argv[i][0] == '-' && strcmp(argv[i] + 1, args[j]->shortarg) == 0) ? 1 : 0) : 0;
      int l = strlen(argv[i]) > 2 ? ((argv[i][0] == '-' && argv[i][1] == '-' && strcmp(argv[i] + 2, args[j]->longarg) == 0) ? 1 : 0) : 0;
      if(s || l){
        m = 1;
        args[j]->count++;
        if(args[j]->takes != NULL){
          args[j]->value = argv[++i];
          if(args[j]->value == NULL){
            r++;
            ERROR("%s: missing argument: %s\n", argv[0], argv[i - 1]);
          }
        }
        break;
      }
    }
    if(!m){
      r++;
      ERROR("%s: invalid flag: %s\n", argv[0], argv[i]);
    }
  }
  return r;
}

int main(int argc, char **argv) {
  // Define command line arguments
  gmk_arg_t help = {"h", "help", NULL, "Display this help message", NULL, 0};
  gmk_arg_t output = {"o", "output", "<file>", "Change the output file destination (default: Makefile)", NULL, 0};
  gmk_arg_t toml = {"t", "toml", "<file>", "Change the TOML file to parse (default: project.toml)", NULL, 0};
  gmk_arg_t version = {"v", "version", NULL, "Display version info", NULL, 0};
  gmk_arg_t init = {"i", "init", NULL, "Generate a project.toml file", NULL, 0};

  int exit_code = EXIT_SUCCESS;
  char *output_file = NULL;
  char *toml_file = NULL;
  int nerrors = 0;

  arrput(args, &help);
  arrput(args, &output);
  arrput(args, &toml);
  arrput(args, &version);
  arrput(args, &init);

  nerrors = argparse(argc, argv);

  // Display help message
  if (help.count > 0) {
    display_help();
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
      ERROR("Failed to initialize project\n");
      exit_code = EXIT_FAILURE;
      goto cleanup;
    }
    goto cleanup;
  }

  // Set file paths
  output_file =
      safe_strdup(output.count > 0 ? output.value : DEFAULT_OUTPUT_FILE);
  toml_file = safe_strdup(toml.count > 0 ? toml.value : DEFAULT_TOML_FILE);

  if (!output_file || !toml_file) {
    ERROR("Memory allocation failed\n");
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

  return exit_code;
}
