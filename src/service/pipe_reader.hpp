#pragma once
#ifdef __APPLE__

#include <string>

class PipeReader {
   public:
    int open();
    std::string read();

   private:
    int m_fd;
};
#endif
