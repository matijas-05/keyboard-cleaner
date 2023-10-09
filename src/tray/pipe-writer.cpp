#include "pipe-writer.hpp"
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <cassert>
#include <cerrno>
#include "config.h"
#include "log.c/log.h"

void PipeWriter::open() {
    m_fd = ::open(PIPE_PATH, O_WRONLY);
    if (m_fd == -1) {
        log_error("Failed to open named pipe: %s", strerror(errno));
    }
}

int PipeWriter::write(const char command[CMD_LEN]) const {
    assert(strlen(command) == CMD_LEN);
    if (::write(m_fd, command, strlen(command)) == -1) {
        return -1;
    }
    return 0;
}
