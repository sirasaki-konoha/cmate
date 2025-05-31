#include <stdlib.h>

#include "toml_utils.h"

/**
 * Get a array of strings from a TOML array.
 * @param arr TOML array to extract strings from
 * @param out_count Pointer to store the number of strings found
 * @return An array of strings, NULL-terminated. Caller must free the array and its contents.
 */
char **get_arrays(toml_array_t *arr, int *out_count) {
  if (!arr)
    return NULL;

  int n = toml_array_nelem(arr);
  char **arrays = malloc(sizeof(char *) * (n + 1));
  if (!arrays)
    return NULL;

  int actual = 0;
  for (int i = 0; i < n; i++) {
    const char *raw = toml_raw_at(arr, i);
    if (!raw)
      continue;

    char *str;
    if (toml_rtos(raw, &str) == 0) {
      arrays[actual++] = str;
    }
  }

  arrays[actual] = NULL;
  if (out_count)
    *out_count = actual;

  return arrays;
}


/**
 * Free an array of strings.
 * @param arr   Array of strings to free
 * @param count Number of strings in the array 
 */
void free_arrays(char **arr, int count) {
  if (!arr) return;
  for (int i = 0; i < count; i++) {
    free(arr[i]);
  }
  free(arr);
}

/**
 * Free an array of toml_parsed_t structures and their contents.
 * @param toml_parsed Array of toml_parsed_t
 * @param count       Number of elements in the array
 */
void free_toml_parsed(toml_parsed_t *toml_parsed, int count) {
  if (!toml_parsed) return;
  for (int i = 0; i < count; i++) {
    int n = 0;
    if (toml_parsed[i].libraries) {
      while (toml_parsed[i].libraries[n]) n++;
      free_arrays(toml_parsed[i].libraries, n);
    }
    n = 0;
    if (toml_parsed[i].includes) {
      while (toml_parsed[i].includes[n]) n++;
      free_arrays(toml_parsed[i].includes, n);
    }
    n = 0;
    if (toml_parsed[i].srcdirs) {
      while (toml_parsed[i].srcdirs[n]) n++;
      free_arrays(toml_parsed[i].srcdirs, n);
    }
    n = 0;
    if (toml_parsed[i].compile_file) {
      while (toml_parsed[i].compile_file[n]) n++;
      free_arrays(toml_parsed[i].compile_file, n);
    }
    free(toml_parsed[i].cflags);
    free(toml_parsed[i].compiler);
    free(toml_parsed[i].project_name);
  }
  free(toml_parsed);
}

/**
 * Get a string value from a TOML table.
 * @param conf TOML table to extract the string from
 * @param name Name of the key to retrieve
 * @return The string value, or NULL if not found. Caller must free the string.
 */
char *get_str(toml_table_t *conf, const char *name) {
  const char *raw_str = toml_raw_in(conf, name);

  if (raw_str) {
    char *unquoted;
    if (toml_rtos(raw_str, &unquoted) == 0) {
      return unquoted;
    }
  }

  return NULL;
}