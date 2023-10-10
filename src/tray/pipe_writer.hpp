#pragma once

#include <fstream>

class PipeWriter {
   public:
    int open();
    int write(std::string command);

   private:
    std::ofstream m_stream;
};
