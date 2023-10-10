#ifndef PIPE_WRITER_HPP
#define PIPE_WRITER_HPP

#include <fstream>
#include "config.h"

class PipeWriter {
   public:
    int open();
    int write(const char command[CMD_LEN]);

   private:
    std::ofstream m_stream;
};

#endif  // !PIPE_WRITER_HPP
