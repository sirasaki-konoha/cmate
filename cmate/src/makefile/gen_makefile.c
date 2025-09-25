
#include "makefile/gen_makefile.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "makefile/embed_mkfile.h"
#include "toml/toml_utils.h"
#include "tomlc99/toml.h"
#include "utils/depend.h"
#include "utils/run_command.h"
#include "utils/term_color.h"
#include "utils/utils.h"

#define info(...)      \
  if (output) {        \
    INFO(__VA_ARGS__); \
  }

int get_array_len(char **array) {
  int len = 0;
  while (array && array[len]) len++;
  return len;
}

char *collect_libs(char **array, const char *base, const char *prefix) {
  if (!array || get_array_len(array) == 0) return NULL;

  char *result = format_string("%s", base);
  if (!result) return NULL;

  int len = get_array_len(array);
  for (int i = 0; i < len; i++) {
    char *tmp = result;
    result = format_string("%s %s%s", result, prefix, array[i]);
    free(tmp);
    if (!result) return NULL;
  }
  return result;
}

char *gen_makefile(toml_parsed_t *parsed, int count, const char *cmate_version,
                   int output) {
  if (count <= 0 || !parsed) {
    ERR("No project configuration found.\n");
    return NULL;
  }

  char *project_names = NULL;
  for (int i = 0; i < count; ++i) {
    if (!parsed[i].project_name) {
      ERR("Project name missing in config %d\n", i);
      free(project_names);
      return NULL;
    }
    char *tmp = project_names;
    project_names = format_string("%s%s%s", tmp ? tmp : "", (tmp ? " " : ""),
                                  parsed[i].project_name);
    free(tmp);
    if (!project_names) {
      ERR("Memory allocation failed for project names\n");
      return NULL;
    }
  }

  char *project_names_line =
      format_string("PROJECT_NAMES := %s", project_names);
  free(project_names);
  if (!project_names_line) {
    ERR("Memory allocation failed for project names line\n");
    return NULL;
  }

  char *all_vars = safe_strdup("");
  if (!all_vars) {
    ERR("Memory allocation failed for all_vars\n");
    free(project_names_line);
    return NULL;
  }

  char *args[] = {"gmake", "--version", NULL};
  if (run_command("gmake", args) != 1) {
    info("make: gmake\n");
  } else {
    ERR("GNU Make is not available!\n");
    ERR("Please install gnu make and run cmate\n");
    free(project_names_line);
    free(all_vars);
    return NULL;
  }

  for (int i = 0; i < count; ++i) {
    toml_parsed_t *p = &parsed[i];

    char *cc = NULL;
    if (!p->compiler || strcmp(p->compiler, "auto") == 0) {
      char *compiler = auto_detect_compiler();
      if (!compiler) {
        ERR("C compiler not found!\n");
        free(all_vars);
        free(project_names_line);
        return NULL;
      }
      cc = format_string("CC_%s := %s", p->project_name, compiler);
      info("%s: Compiler = %s (auto)\n", p->project_name, compiler);
    } else {
      char *result = check_depends(p);
      if (!result) {
        free(all_vars);
        free(project_names_line);
        return NULL;
      }
      cc = format_string("CC_%s := %s", p->project_name, p->compiler);
      info("%s: Compiler = %s\n", p->project_name, p->compiler);
      free(result);
    }

    if (!cc) {
      ERR("Memory allocation failed for compiler\n");
      free(all_vars);
      free(project_names_line);
      return NULL;
    }

    char *cflags = format_string("CFLAGS_%s := %s", p->project_name,
                                 p->cflags ? p->cflags : "");
    if (!cflags) {
      ERR("Memory allocation failed for cflags\n");
      free(cc);
      free(all_vars);
      free(project_names_line);
      return NULL;
    }

    char *ldlibs_base = format_string("LDLIBS_%s :=", p->project_name);
    char *ldlibs = collect_libs(p->libraries, ldlibs_base, "-l");
    free(ldlibs_base);
    if (!ldlibs) {
      ldlibs = format_string("LDLIBS_%s :=", p->project_name);
    }
    if (!ldlibs) {
      ERR("Memory allocation failed for ldlibs\n");
      free(cc);
      free(cflags);
      free(all_vars);
      free(project_names_line);
      return NULL;
    }

    char *ldflags_base = format_string("LDFLAGS_%s :=", p->project_name);
    char *ldflags = collect_libs(p->ldflags, ldflags_base, " ");
    free(ldflags_base);
    if (!ldflags) {
      ldflags = format_string("LDFLAGS_%s :=", p->project_name);
    }
    if (!ldflags) {
      ERR("Memory allocation failed for ldflags\n");
      free(cc);
      free(cflags);
      free(ldlibs);
      free(all_vars);
      free(project_names_line);
      return NULL;
    }

    char *srcdirs_base = format_string("SRCDIRS_%s :=", p->project_name);
    char *srcdirs = collect_libs(p->srcdirs, srcdirs_base, " ");
    free(srcdirs_base);
    if (!srcdirs) {
      srcdirs = format_string("SRCDIRS_%s := src", p->project_name);
    }
    if (!srcdirs) {
      ERR("Memory allocation failed for srcdirs\n");
      free(cc);
      free(cflags);
      free(ldlibs);
      free(ldflags);
      free(all_vars);
      free(project_names_line);
      return NULL;
    }

    char *includes_base = format_string("INCLUDE_DIRS_%s :=", p->project_name);
    char *includes = collect_libs(p->includes, includes_base, " ");
    free(includes_base);
    if (!includes) {
      includes = format_string("INCLUDE_DIRS_%s := include", p->project_name);
    }
    if (!includes) {
      ERR("Memory allocation failed for includes\n");
      free(cc);
      free(cflags);
      free(ldlibs);
      free(ldflags);
      free(srcdirs);
      free(all_vars);
      free(project_names_line);
      return NULL;
    }

    char *compile_files = NULL;
    if (p->compile_file && get_array_len(p->compile_file) > 0) {
      char *compile_files_base =
          format_string("EXTRA_SOURCES_%s :=", p->project_name);
      compile_files = collect_libs(p->compile_file, compile_files_base, " ");
      free(compile_files_base);
    }
    if (!compile_files) {
      compile_files = format_string("EXTRA_SOURCES_%s :=", p->project_name);
    }
    if (!compile_files) {
      ERR("Memory allocation failed for compile_files\n");
      free(cc);
      free(cflags);
      free(ldlibs);
      free(ldflags);
      free(srcdirs);
      free(includes);
      free(all_vars);
      free(project_names_line);
      return NULL;
    }

    char *tmp = all_vars;
    all_vars =
        format_string("%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n", tmp, cc, cflags,
                      ldlibs, ldflags, srcdirs, includes, compile_files);
    free(tmp);
    free(cc);
    free(cflags);
    free(ldlibs);
    free(ldflags);
    free(srcdirs);
    free(includes);
    free(compile_files);

    if (!all_vars) {
      ERR("Memory allocation failed for all_vars concatenation\n");
      free(project_names_line);
      return NULL;
    }
  }

  char *copy = malloc(template_Makefile_len + 1);
  if (!copy) {
    ERR("Makefile template alloc fail\n");
    free(all_vars);
    free(project_names_line);
    return NULL;
  }
  memcpy(copy, template_Makefile, template_Makefile_len);
  copy[template_Makefile_len] = '\0';

  char *first = format_string("# @generated by cmate %s\n", cmate_version);
  if (!first) {
    ERR("Memory allocation failed for first line\n");
    free(all_vars);
    free(project_names_line);
    free(copy);
    return NULL;
  }

  char *result = format_string("%s\n%s\n%s\n%s", first, project_names_line,
                               all_vars, copy);
  if (!result) {
    ERR("Memory allocation failed for final result\n");
    free(all_vars);
    free(project_names_line);
    free(copy);
    free(first);
    return NULL;
  }

  if (output) printf("Makefile generated\n");

  free(project_names_line);
  free(all_vars);
  free(copy);
  free(first);
  return result;
}

#undef info
