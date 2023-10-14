#pragma once

#if defined(__linux__) || defined(__APPLE__)
#define PIPE_PATH "/tmp/keyboard-cleaner-fifo"
#elif defined(_WIN32)
// TODO: Implement
#endif

#ifdef __linux__
#define CMD_LEN 64  // because on Linux we also pass keyboard path when enabling
#else
#define CMD_LEN 1
#endif

#define DISABLE "d"
#define ENABLE "e"
