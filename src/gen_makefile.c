#include <embed_mkfile.h>
#include <gen_makefile.h>
#include <gen_toml.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <toml.h>
#include <utils.h>

char *gen_makefile(toml_parsed_t parsed) {

  char *cc = format_string("CC     := %s", parsed.compiler);
  printf("=> Compile flags is %s\n", parsed.cflags);
  char *cflags = format_string("CFLAGS := %s", parsed.cflags);
  char *ldflags = format_string("LDLIBS	:=");

  int len = 0;
  while (parsed.libraries[len])
    len++;

  for (int i = 0; i < len; i++) {
    if (i == 0) {
      printf("=> libraries is ");
    }
    char *tmp = ldflags;
    ldflags = format_string("%s -l%s", ldflags, parsed.libraries[i]);

    if (i != len - 1) {
      printf("%s, ", parsed.libraries[i]);
    } else {
      printf("%s", parsed.libraries[i]);
    }

    free(tmp);
  }
  printf("\n");

  char *project_name =
      format_string("PROJECT_NAME     := %s", parsed.project_name);
  printf("=> project name is %s\n", parsed.project_name);

  char *copy = malloc(template_Makefile_len + 1);
  memcpy(copy, template_Makefile, template_Makefile_len);
  copy[template_Makefile_len] = '\0';

  char *prev = format_string("%s\n%s\n%s\n%s\n%s\n", cc, cflags, ldflags,
                             project_name, copy);

  free(cc);
  free(cflags);
  free(ldflags);
  free(project_name);
  free(copy);

  return prev;
}
