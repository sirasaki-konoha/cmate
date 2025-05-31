#ifdef _WIN32
#include <windows.h>
#undef ERROR
#include <stdio.h>
#include "term_color.h"

void enable_ansi_escape_codes() {
  HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
  if (hOut == INVALID_HANDLE_VALUE) {
    ERROR("Failed to get stdout\n");
    return;
  }

  DWORD dwMode = 0;
  if (!GetConsoleMode(hOut, &dwMode)) {
    ERROR("Failed to get console mode\n");
    return;
  }

  dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
  if (!SetConsoleMode(hOut, dwMode)) {
    ERROR("Failed to set console Mode\n");
    return;
  }
}

#else
void enable_ansi_escape_codes() {}

#endif // _WIN32