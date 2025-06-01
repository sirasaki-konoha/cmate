#include "main/main.h"
#include "main/init.h"
#include "term_color.h"

/**
 * Create source directory and main.c file
 * @return 0 on success, non-zero on failure
 */
int create_source_files(void) {
  int result = 0;

  // Create source directory
  if (MKDIR(SRC_DIRECTORY) != 0) {
    // Not an error if directory already exists
    if (errno != EEXIST) {
      ERR("Failed to create source directory\n");
      return 1;
    }
  }
  INFO("%s: OK\n", SRC_DIRECTORY);

  // Create include directory
  if (MKDIR(INCLUDE_DIR) != 0) {
    // Not an error if directory already exists
    if (errno != EEXIST) {
      ERR("Failed to create include directory\n");
      return 1;
    }
  }
  INFO("%s: OK\n", INCLUDE_DIR);

  // Allocmate memory for main.c template
  char *main_template = malloc(template_main_template_len + 1);
  if (!main_template) {
    ERR("Memory allocation failed for template\n");
    return 1;
  }
  if (create_template_not_exits(SRC_DIRECTORY, MAIN_C_FILE,
                                (const char *)template_main_template,
                                template_main_template_len) != 0) {
    return 1;
  }

  INFO("%s/%s: OK\n", SRC_DIRECTORY, MAIN_C_FILE);

  if (create_template_not_exits(NULL, ".gitignore",
                                (const char *)template_gitignore_template,
                                template_Makefile_len) != 0) {
    return 1;
  }
  INFO("%s: OK\n", ".gitignore");

  return result;
}

/**
 * Create a template file if it does not exist
 * @param dir Directory to create the file in, or NULL for current directory
 * @param filename Name of the file to create
 * @param template Content of the template to write
 * @param template_len Length of the template content
 * @return 0 on success, non-zero on failure
 */
int create_template_not_exits(const char *dir, const char *filename,
                                     const char *template,
                                     int const template_len) {
  char *template_text = malloc(template_len + 1);

  if (template_text == NULL) {
    return 1;
  }

  memcpy(template_text, template, template_len);
  template_text[template_len] = '\0';

  // Build file path
  char filepath[256];
#ifdef _WIN32
  if (dir != NULL) {
    snprintf(filepath, sizeof(filepath), "%s\\%s", dir, filename);
  } else {
    snprintf(filepath, sizeof(filepath), "%s", filename);
  }
#else
  if (dir != NULL) {
    snprintf(filepath, sizeof(filepath), "%s/%s", dir, filename);
  } else {
    snprintf(filepath, sizeof(filepath), "%s", filename);
  }
#endif

  // Create file and write content
  if (create_not_exists(filepath, template_text) != 0) {
    return 1;
  }
  free(template_text);
  return 0;
}