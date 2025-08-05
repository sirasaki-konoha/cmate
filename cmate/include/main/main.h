#ifndef _MAIN_H
#define _MAIN_H

/* Cross-platform support */
# ifdef _WIN32
#  include <direct.h>
#  include <windows.h>

#  include "windows_support.h"
#  define MKDIR(dir) _mkdir(dir)
#  define chdir(x) _chdir(x)
#  define getcwd(x, y) _getcwd(x, y)

# else
#  include "unistd.h"
#  include "utils/term_color.h"
#  include <sys/stat.h>
#  include <sys/types.h>

#  define MKDIR(dir) mkdir(dir, 0755)
# endif

# include <errno.h>
# include <utils/stb_ds.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
/* cmate */
# include "build/build.h"
# include "run/run.h"
# include "cmate_arg_t.h"
# include "makefile/embed_mkfile.h"
# include "file_io/file_io.h"
# include "makefile/gen_makefile.h"
# include "main/arg.h"
# include "main/init.h"
# include "main/process_makefile.h"
# include "toml/toml_utils.h"
# include "utils/utils.h"
# include "windows/windows_support.h"
# include "toml/find_toml.h"

/* CMATE version settings etc. */

# define CMATE_VERSION "1.2"

# define CMATE_COPYRIGHT                                                        \
  "Copyright (C) 2025 noa-vliz\nModified by rock-db (c) 2025"
# define CMATE_LICENSE "Licensed under the MIT License."
# define CMATE_SOURCE "Source: https://github.com/rock-db/cmate"

# define DEFAULT_OUTPUT_FILE "Makefile"
# define DEFAULT_TOML_FILE "Cmate.toml"
# define SRC_DIRECTORY "src"
# define INCLUDE_DIR "include"
# define MAIN_C_FILE "main.c"

#endif /* _MAIN_H */
