#pragma once

#if defined(__linux__) || defined(__APPLE__)
#define PIPE_PATH "/tmp/keyboard-cleaner-fifo"
#elif defined(_WIN32)
// TODO: Implement
#endif

#define CMD_LEN 1

#define DISABLE "d"
#define ENABLE "e"
