#include "main/main.h"
#include "utils/term_color.h"

/**
 * Process TOML file and generate Makefile
 * @param toml_file Path to TOML file
 * @param output_file Path to output file
 * @return 0 on success, non-zero on failure
 */
int process_makefile(const char *toml_file, const char *output_file,
                     int output) {
  if (!toml_file || !output_file) {
    ERR("Invalid file paths\n");
    return 1;
  }

  int count = 0;
  // Parse TOML file
  toml_parsed_t *tml = parse_toml(toml_file, &count);

  // check librarys
  // if (check_librarys(tml, count, CMATE_VERSION, output){}

  // Generate Makefile content
  char *makefile_content = gen_makefile(tml, count, CMATE_VERSION, output);

  if (!makefile_content) {
    ERR("Failed to generate Makefile content\n");
    free_toml_parsed(tml, count);
    return 1;
  }

  // Create and write Makefile
  if (create_and_write(output_file, makefile_content) != 0) {
    ERR("Failed to write to output file: %s\n", output_file);
    free(makefile_content);
    free_toml_parsed(tml, count);
    return 1;
  }

  free(makefile_content);
  free_toml_parsed(tml, count);

  return 0;
}
