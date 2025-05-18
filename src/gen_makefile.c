#include <embed_mkfile.h>
#include <gen_makefile.h>
#include <gen_toml.h>
#include <stdlib.h>
#include <string.h>
#include <toml.h>
#include <utils.h>

char *gen_makefile(toml_parsed_t parsed) {
  char *cc = format_string("CC     := %s", parsed.compiler);
  char *cflags = format_string("CFLAGS := %s", parsed.cflags);

  int len = 0;
  while (parsed.libraries[len])
    len++;

  for (int i = 0; i < len; i++) {
    char *tmp = cflags;
    cflags = format_string("%s -l%s ", tmp, parsed.libraries[i]);
    free(tmp);
  }

  char *project_name =
      format_string("PROJECT_NAME     := %s", parsed.project_name);

  char *copy = malloc(template_Makefile_len + 1);
  memcpy(copy, template_Makefile, template_Makefile_len);
  copy[template_Makefile_len] = '\0';

  char *prev = format_string("%s\n%s", cc, cflags);
  char *next = format_string("%s\n%s", prev, project_name);
  free(prev);

  char *end = format_string("%s\n%s", next, copy);
  free(next);

  free_arrays(parsed.libraries, len);
  free(parsed.project_name);
  free(parsed.cflags);
  free(parsed.compiler);

  return end;
}
