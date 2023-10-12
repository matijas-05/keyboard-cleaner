#pragma once

#include <string>

class PipeReader {
   public:
    int open();
    std::string read();

   private:
    int m_fd;
};
