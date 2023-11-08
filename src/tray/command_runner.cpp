#include "command_runner.hpp"
#include "../service/service.hpp"

#ifdef __APPLE__
#include "../config.hpp"
int CommandRunner::open() {
    m_stream = std::ofstream(PIPE_PATH);
    if (!m_stream) {
        return -1;
    }
    return 0;
}
#endif

int CommandRunner::run(std::string command) {
#if defined(__APPLE__)
    m_stream << command << std::flush;
    if (!m_stream) {
        return -1;
    }
    return 0;
#else
    runCommand(command);
    return 0;
#endif
}
