#ifndef _MAIN_H
#define _MAIN_H

/* Cross-platform support */
#ifdef _WIN32
#include <direct.h>
#include <windows.h>

#include "windows_support.h"
#define MKDIR(dir) _mkdir(dir)

#include "visual_studio/sln/generate.h"
#include "visual_studio/vcxproj/generate.h"

#else
#include "term_color.h"
#include <sys/stat.h>
#include <sys/types.h>

#define MKDIR(dir) mkdir(dir, 0755)
#endif

#include <errno.h>
#include <stb_ds.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/* cmate */
#include "build.h"
#include "cmate_arg_t.h"
#include "embed_mkfile.h"
#include "file_io.h"
#include "gen_makefile.h"
#include "main/arg.h"
#include "main/init.h"
#include "main/process_makefile.h"
#include "toml_utils.h"
#include "utils.h"
#include "windows_support.h"

/* CMATE version settings etc. */

#define CMATE_VERSION "1.1"
#define CMATE_COPYRIGHT                                                        \
  "Copyright (C) 2025 noa-vliz\nModified by rock-db (c) 2025"
#define CMATE_LICENSE "Licensed under the MIT License."
#define CMATE_SOURCE "Source: https://github.com/rock-db/cmate"

#define DEFAULT_OUTPUT_FILE "Makefile"
#define DEFAULT_TOML_FILE "Cmate.toml"
#define SRC_DIRECTORY "src"
#define INCLUDE_DIR "include"
#define MAIN_C_FILE "main.c"

#endif /* _MAIN_H */
