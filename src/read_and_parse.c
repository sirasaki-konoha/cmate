#include <file_io.h>
#include <gen_toml.h>
#include <stdio.h>
#include <stdlib.h>
#include <toml.h>

toml_parsed_t *read_and_parse(const char *path, int *out_count) {
    FILE *file = fopen(path, "r");
    if (file == NULL) {
        fprintf(stderr, "Unable to open %s!\n", path);
        exit(EXIT_FAILURE);
    }

    char errbuf[300];
    toml_table_t *conf = toml_parse_file(file, errbuf, sizeof(errbuf));
    fclose(file);

    if (!conf) {
        fprintf(stderr, "TOML parse error: %s\n", errbuf);
        exit(EXIT_FAILURE);
    }

    toml_array_t *binary_projects = toml_array_in(conf, "bin");
    int bin_count = binary_projects ? toml_array_nelem(binary_projects) : 0;
    if (out_count) *out_count = bin_count;

    toml_parsed_t *parsed = calloc(bin_count, sizeof(toml_parsed_t));
    if (!parsed) {
        fprintf(stderr, "Memory allocation failed!\n");
        toml_free(conf);
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < bin_count; i++) {
        toml_table_t *bin_tbl = toml_table_at(binary_projects, i);
        int count = 0;

        parsed[i].project_name = get_str(bin_tbl, "project");
        parsed[i].compiler = get_str(bin_tbl, "compiler");
        parsed[i].cflags = get_str(bin_tbl, "flags");
        parsed[i].compile_file = get_arrays(toml_array_in(bin_tbl, "compile"), &count);
        count = 0;
        parsed[i].includes = get_arrays(toml_array_in(bin_tbl, "include"), &count);
        count = 0;
        parsed[i].srcdirs = get_arrays(toml_array_in(bin_tbl, "src"), &count);
        count = 0;
        parsed[i].libraries = get_arrays(toml_array_in(bin_tbl, "libs"), &count);
    }

    toml_free(conf);
    return parsed;
}
