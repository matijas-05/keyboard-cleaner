#pragma once

#include <fstream>

class PipeWriter {
   public:
#ifdef __APPLE__
    int open();
#endif
    int write(std::string command);

#ifdef __APPLE__
   private:
    std::ofstream m_stream;
#endif
};
