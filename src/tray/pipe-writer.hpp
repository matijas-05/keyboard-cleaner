#ifndef PIPE_WRITER_HPP
#define PIPE_WRITER_HPP

#include "config.h"

class PipeWriter {
   public:
    PipeWriter() : m_fd(-1){};
    void open();
    int write(const char command[CMD_LEN]) const;

   private:
    int m_fd;
};

#endif  // !PIPE_WRITER_HPP
