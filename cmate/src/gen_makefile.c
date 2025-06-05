#include "tomlc99/toml.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "depend.h"
#include "embed_mkfile.h"
#include "gen_makefile.h"
#include "term_color.h"
#include "toml_utils.h"
#include "utils.h"

#define info(...)                                                              \
  if (output) {                                                                \
    INFO(__VA_ARGS__);                                                         \
  }

int get_array_len(char **array) {
  int len = 0;
  while (array != NULL && array[len])
    len++;
  return len;
}

char *display_and_collect_libs(char **array, const char *message,
                               const char *base, const char *prefix,
                               int output) {
  char *result = format_string("%s", base);

  int len = get_array_len(array);
  if (len > 0) {
    if (output)
      INFO("%s", message);

    for (int i = 0; i < len; i++) {
      char *tmp = result;
      result = format_string("%s %s%s", result, prefix, array[i]);

      if (output) {
        if (i != len - 1) {
          printf("%s, ", array[i]);
        } else {
          printf("%s", array[i]);
        }
      }

      free(tmp);
    }
    if (output)
      printf("\n");
  } else {
    if (output)
      INFO("%s (none)\n", message);
    free(result);
    return NULL;
  }

  return result;
}

char *gen_makefile(toml_parsed_t *parsed, int count, const char *cmate_version,
                   int output) {

  if (count <= 0 || parsed == NULL) {
    ERR("No project configuration found.\n");
    return NULL;
  }

  char *project_names = NULL;
  for (int i = 0; i < count; ++i) {
    if (!parsed[i].project_name) {
      ERR("Project name not specified in configuration for entry %d\n", i);
      return NULL;
      break;
    }

    if (i == 0) {
      info("Project name(s): ");
    }
    char *tmp = project_names;
    project_names = format_string("%s%s%s", tmp ? tmp : "", (tmp ? " " : ""),
                                  parsed[i].project_name);

    if (output) {
      if (i == (count - 1)) {
        printf("%s", parsed[i].project_name);
      } else {
        printf("%s, ", parsed[i].project_name);
      }
    }

    if (tmp)
      free(tmp);
  }
  if (output)
    printf("\n");

  char *project_names_line =
      format_string("PROJECT_NAMES := %s", project_names);

  free(project_names);

  char *all_vars = safe_strdup("");
  for (int i = 0; i < count; ++i) {
    toml_parsed_t *p = &parsed[i];
    if (output)
      printf("%s %s %s\n", YELLOW("===="), p->project_name, YELLOW("===="));

    char *cc = NULL;
    if (p->compiler == NULL || strcmp(p->compiler, "auto") == 0) {
      char *compiler = auto_detect_compiler();
      if (compiler == NULL) {
        ERR("The C compiler is not installed!\n");
        info("%s: The C compiler is not installed!\n", p->project_name);
        free(all_vars);
        free(project_names_line);
        return NULL;
      }
      cc = format_string("CC_%s := %s", p->project_name, compiler);
      info("%s: The C compiler is %s(auto)\n", p->project_name, compiler);
    } else {
      char *result = check_depends(p);
      if (result == NULL) {
        free(all_vars);
        free(project_names_line);
        return NULL;
      }
      cc = format_string("CC_%s := %s", p->project_name, p->compiler);
      free(result);
    }

    char *cflags = format_string("CFLAGS_%s := %s", p->project_name,
                                 p->cflags ? p->cflags : "");

    char *ldlibs = display_and_collect_libs(
        p->libraries, format_string("%s: Libraries: ", p->project_name),
        format_string("LDLIBS_%s :=", p->project_name), "-l", output);
    if (!ldlibs)
      ldlibs = format_string("LDLIBS_%s :=", p->project_name);

    char *ldflags = display_and_collect_libs(
        p->ldflags, format_string("%s: Linker flags: ", p->project_name),
        format_string("LDFLAGS_%s :=", p->project_name), " ", output);

    if (!ldflags)
      ldflags = format_string("LDFLAGS_%s :=", p->project_name);

    char *srcdirs = display_and_collect_libs(
        p->srcdirs, format_string("%s: Source directory: ", p->project_name),
        format_string("SRCDIRS_%s :=", p->project_name), " ", output);
    if (!srcdirs)
      srcdirs = format_string("SRCDIRS_%s := src", p->project_name);

    char *includes = display_and_collect_libs(
        p->includes, format_string("%s: Include directory: ", p->project_name),
        format_string("INCLUDE_DIRS_%s :=", p->project_name), " ", output);
    if (!includes)
      includes = format_string("INCLUDE_DIRS_%s := include", p->project_name);

    char *compile_files = display_and_collect_libs(
        p->compile_file, format_string("%s: Extra sources: ", p->project_name),
        format_string("EXTRA_SOURCES_%s :=", p->project_name), " ", output);
    if (!compile_files)
      compile_files = format_string("EXTRA_SOURCES_%s :=", p->project_name);

    char *tmp = all_vars;
    all_vars = format_string("\n"
                             "CMATE_VERSION := %s\n"
                             "%s\n"
                             "%s\n"
                             "%s\n"
                             "%s\n"
                             "%s\n"
                             "%s\n"
                             "%s\n"
                             "%s\n",
                             cmate_version, tmp, cc, cflags, ldlibs, ldflags,
                             srcdirs, includes, compile_files);
    free(tmp);
    free(cc);
    free(cflags);
    free(ldlibs);
    free(srcdirs);
    free(includes);
    free(compile_files);
  }

  char *copy = malloc(template_Makefile_len + 1);
  if (!copy) {
    ERR("Failed to allocate memory for Makefile template\n");
    free(all_vars);
    free(project_names_line);
    return NULL;
  }
  memcpy(copy, template_Makefile, template_Makefile_len);
  copy[template_Makefile_len] = '\0';

  char *first = format_string("# @generated by cmate %s.\n"
                              "# Do not edit this file manually.\n"
                              "# To build all binaries, run: make\n"
                              "# Report issues or contribute at: "
                              "https://github.com/rock-db/cmate\n",
                              cmate_version);

  char *result = format_string("%s\n%s\n%s\n%s\n", first, project_names_line,
                               all_vars, copy);

  if (output)
    printf("[SUCCESS] Makefile generated successfully\n");

  free(project_names_line);
  free(all_vars);
  free(copy);
  free(first);

  return result;
}

#undef info
