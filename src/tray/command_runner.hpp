#pragma once

#include <fstream>

class CommandRunner {
   public:
#ifdef __APPLE__
    int open();
#endif
    int run(std::string command);

#ifdef __APPLE__
   private:
    std::ofstream m_stream;
#endif
};
