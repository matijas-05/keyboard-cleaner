#ifdef __APPLE__
#include "pipe_reader.hpp"
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include "../config.hpp"
#include "log.c/log.h"

int PipeReader::open() {
    m_fd = ::open(PIPE_PATH, O_RDONLY);
    if (m_fd == -1 && errno != ENOENT) {
        return -1;
    }

    // Wait until tray creates named pipe
    // Tray has to do it because it has lower permissions
    if (errno == ENOENT) {
        log_debug("Waiting for tray to create named pipe...");
    }
    while (errno == ENOENT || errno == ETIMEDOUT) {
        m_fd = ::open(PIPE_PATH, O_RDONLY);
        if (m_fd != -1) {
            break;
        }
        sleep(1);
    }
    return 0;
}

std::string PipeReader::read() {
    char buf[CMD_LEN];
    int n = ::read(m_fd, buf, CMD_LEN);
    if (n == -1) {
        log_error("Failed to read from named pipe: %s", std::strerror(errno));
        return "";
    }
    return std::string(buf, n);
}
#endif
