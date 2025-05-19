#include <embed_mkfile.h>
#include <gen_makefile.h>
#include <gen_toml.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <toml.h>
#include <utils.h>

char *gen_makefile(toml_parsed_t parsed) {
  if (parsed.compiler == NULL) {
    printf("[ERROR] Compiler not specified in configuration\n");
    return NULL;
  }

  char *cc = format_string("CC     := %s", parsed.compiler);

  printf("[INFO] Compile flags: %s\n",
         parsed.cflags ? parsed.cflags : "(none)");
  char *cflags =
      format_string("CFLAGS := %s", parsed.cflags ? parsed.cflags : "");

  char *ldflags = format_string("LDLIBS	:=");

  int len = 0;
  while (parsed.libraries != NULL && parsed.libraries[len])
    len++;

  if (len > 0) {
    printf("[INFO] Libraries: ");

    for (int i = 0; i < len; i++) {
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
  } else {
    printf("[INFO] Libraries: (none)\n");
  }

  if (parsed.project_name == NULL) {
    printf("[ERROR] Project name not specified in configuration\n");
    free(cc);
    free(cflags);
    free(ldflags);
    return NULL;
  }

  printf("[INFO] Project name: %s\n", parsed.project_name);
  char *project_name =
      format_string("PROJECT_NAME     := %s", parsed.project_name);

  char *copy = malloc(template_Makefile_len + 1);
  if (copy == NULL) {
    printf("[ERROR] Failed to allocate memory for Makefile template\n");
    free(cc);
    free(cflags);
    free(ldflags);
    free(project_name);
    return NULL;
  }

  memcpy(copy, template_Makefile, template_Makefile_len);
  copy[template_Makefile_len] = '\0';

  char *prev = format_string("%s\n%s\n%s\n%s\n%s\n", cc, cflags, ldflags,
                             project_name, copy);

  printf("[SUCCESS] Makefile generated successfully\n");

  free(cc);
  free(cflags);
  free(ldflags);
  free(project_name);
  free(copy);

  return prev;
}
