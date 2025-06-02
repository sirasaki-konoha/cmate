#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"

#ifdef _WIN32
#include <windows.h>
#define strcasecmp _stricmp
#define PATH_SEP '\\'
#else
#include <dirent.h>
#include <sys/stat.h>
#define PATH_SEP '/'
#endif

#define INITIAL_CAPACITY 64

static void add_file(char ***files, int *count, int *capacity, const char *filepath) {
    if (*count >= *capacity) {
        *capacity *= 2;
        *files = realloc(*files, (*capacity) * sizeof(char *));
    }
    (*files)[*count] = safe_strdup(filepath);
    (*count)++;
}

#ifdef _WIN32

static void search_dir(const char *dir, const char *ext, char ***files, int *count, int *capacity) {
    char search_path[MAX_PATH];
    snprintf(search_path, MAX_PATH, "%s\\*", dir);

    WIN32_FIND_DATAA fd;
    HANDLE hFind = FindFirstFileA(search_path, &fd);
    if (hFind == INVALID_HANDLE_VALUE) return;

    do {
        if (strcmp(fd.cFileName, ".") == 0 || strcmp(fd.cFileName, "..") == 0) continue;

        char full_path[MAX_PATH];
        snprintf(full_path, MAX_PATH, "%s\\%s", dir, fd.cFileName);

        if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            search_dir(full_path, ext, files, count, capacity);
        } else {
            const char *dot = strrchr(fd.cFileName, '.');
            if (dot && strcasecmp(dot, ext) == 0) {
                add_file(files, count, capacity, full_path);
            }
        }
    } while (FindNextFileA(hFind, &fd));
    FindClose(hFind);
}

#else

static void search_dir(const char *dir, const char *ext, char ***files, int *count, int *capacity) {
    DIR *dp = opendir(dir);
    if (!dp) return;
    struct dirent *entry;
    while ((entry = readdir(dp)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;

        char full_path[4096];
        snprintf(full_path, sizeof(full_path), "%s/%s", dir, entry->d_name);

        struct stat st;
        if (stat(full_path, &st) == -1) continue;

        if (S_ISDIR(st.st_mode)) {
            search_dir(full_path, ext, files, count, capacity);
        } else {
            const char *dot = strrchr(entry->d_name, '.');
            if (dot && strcasecmp(dot, ext) == 0) {
                add_file(files, count, capacity, full_path);
            }
        }
    }
    closedir(dp);
}

#endif

char **get_files_recursively(const char **dirs, const char *ext, int *count) {
    int capacity = INITIAL_CAPACITY;
    char **files = malloc(capacity * sizeof(char *));
    *count = 0;

    for (int i = 0; dirs[i] != NULL; i++) {
        search_dir(dirs[i], ext, &files, count, &capacity);
    }




    return files;
}




