#include "pipe_writer.hpp"
#include "config.h"

int PipeWriter::open() {
    m_stream = std::ofstream(PIPE_PATH);
    if (m_stream.bad()) {
        return -1;
    }
    return 0;
}

int PipeWriter::write(std::string command) {
    m_stream << command << std::flush;
    if (m_stream.fail()) {
        return -1;
    }
    return 0;
}
