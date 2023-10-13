#pragma once

#include <fstream>

class PipeWriter {
   public:
    int open();
    int write(std::string command);

#if defined(__linux__) || defined(__APPLE__)
   private:
    std::ofstream m_stream;
#elif defined(_WIN32)
    // TODO: Implement
#endif
};
