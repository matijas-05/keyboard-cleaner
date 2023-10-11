#include "pipe_writer.hpp"
#include "../config.hpp"

int PipeWriter::open() {
    m_stream = std::ofstream(PIPE_PATH);
    if (!m_stream) {
        return -1;
    }
    return 0;
}

int PipeWriter::write(std::string command) {
    m_stream << command << std::flush;
    if (!m_stream) {
        return -1;
    }
    return 0;
}
