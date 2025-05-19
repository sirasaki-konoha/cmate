#include <stdio.h>
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

  si.dwFlags = STARTF_USESTDHANDLES;

  HANDLE nullHandle = CreateFile("NUL", GENERIC_WRITE, 0, NULL, OPEN_EXISTING,
                                 FILE_ATTRIBUTE_NORMAL, NULL);

  if (nullHandle == INVALID_HANDLE_VALUE) {
    fprintf(stderr, "Failed to open NUL device\n");
    return -1;
  }

  si.hStdOutput = nullHandle;
  si.hStdError = nullHandle;

  si.hStdInput = GetStdHandle(STD_INPUT_HANDLE);

  ZeroMemory(&pi, sizeof(pi));

  // CreateProcess wants a modifiable command line buffer
  char commandLineMutable[4096];
  strcpy(commandLineMutable, commandLine);

  BOOL success = CreateProcess(NULL, commandLineMutable, NULL, NULL, TRUE, 0,
                               NULL, NULL, &si, &pi);

  CloseHandle(nullHandle);

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

  if (exitCode != 0) {
    return 1;
  }
  return 0;
}

#else
// POSIX
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int run_command(const char *cmd, char *const argv[]) {
  pid_t pid = fork();
  if (pid == 0) {

    int devnull = open("/dev/null", O_WRONLY);
    if (devnull == -1) {
      perror("Failed to open /dev/null");
      _exit(127);
    }

    dup2(devnull, STDOUT_FILENO);
    dup2(devnull, STDERR_FILENO);
    close(devnull);

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
