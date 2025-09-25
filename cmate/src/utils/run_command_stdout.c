#include <stdio.h>

#ifdef _WIN32
#include <windows.h>

int run_command_stdout(const char *cmd, char *const argv[]) {
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

  char commandLineMutable[4096];
  strcpy(commandLineMutable, commandLine);

  BOOL success = CreateProcess(NULL, commandLineMutable, NULL, NULL, TRUE, 0,
                               NULL, NULL, &si, &pi);

  if (!success) {
    return -1;
  }

  WaitForSingleObject(pi.hProcess, INFINITE);

  DWORD exitCode;
  GetExitCodeProcess(pi.hProcess, &exitCode);

  CloseHandle(pi.hProcess);
  CloseHandle(pi.hThread);

  if (exitCode != 0) {
    return 1;
  }
  return 0;
}

#else
// POSIX
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int run_command_stdout(const char *cmd, char *const argv[]) {
  pid_t pid = fork();
  if (pid == 0) {
    execvp(cmd, argv);
    perror("execvp failed");
    _exit(127);
  } else if (pid > 0) {
    int status;
    waitpid(pid, &status, 0);
    if (WIFEXITED(status)) {
      int exitCode = WEXITSTATUS(status);
      return (exitCode != 0) ? 1 : 0;
    } else {
      return 1;
    }
  } else {
    perror("fork failed");
    return -1;
  }
}
#endif
