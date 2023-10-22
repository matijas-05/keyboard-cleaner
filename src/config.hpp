#pragma once

#if defined(__APPLE__)
#define PIPE_PATH "/tmp/keyboard-cleaner-fifo"
#endif

#ifdef __linux__
#define CMD_LEN 64  // because on Linux we also pass keyboard path when enabling
#else
#define CMD_LEN 1
#endif

#define DISABLE "d"
#define ENABLE "e"
