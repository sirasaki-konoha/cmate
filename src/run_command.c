#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <utils.h>

#ifdef _WIN32
#include <windows.h>

int run_command(const char *cmd, char *const argv[]) {
  // Create the command line string
  char commandLine[4096] = {0};
  strcat(commandLine, cmd);
  for (int i = 1; argv[i] != NULL; i++) {
    strcat(commandLine, " ");
    strcat(commandLine, argv[i]);
  }

  STARTUPINFO si;
  PROCESS_INFORMATION pi;
  ZeroMemory(&si, sizeof(si));
  si.cb = sizeof(si);
  ZeroMemory(&pi, sizeof(pi));

  // CreateProcess wants a modifiable command line buffer
  char commandLineMutable[4096];
  strcpy(commandLineMutable, commandLine);

  BOOL success = CreateProcess(NULL, commandLineMutable, NULL, NULL, FALSE, 0,
                               NULL, NULL, &si, &pi);

  if (!success) {
    fprintf(stderr, "CreateProcess failed\n");
    return -1;
  }

  // Wait until child process exits.
  WaitForSingleObject(pi.hProcess, INFINITE);

  DWORD exitCode;
  GetExitCodeProcess(pi.hProcess, &exitCode);

  CloseHandle(pi.hProcess);
  CloseHandle(pi.hThread);

  return (int)exitCode;
}

#else
// POSIX
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int run_command(const char *cmd, char *const argv[]) {
  pid_t pid = fork();
  if (pid == 0) {
    execvp(cmd, argv);
    perror("execvp failed");
    _exit(127);
  } else if (pid > 0) {
    int status;
    waitpid(pid, &status, 0);
    if (WIFEXITED(status)) {
      return WEXITSTATUS(status);
    } else {
      return -1;
    }
  } else {
    perror("fork failed");
    return -1;
  }
}
#endif
