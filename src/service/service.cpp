#include <fcntl.h>
#include <poll.h>
#include <signal.h>
#include <unistd.h>
#include <cstring>
#include <thread>
#include "../config.hpp"
#include "log.c/log.h"
#include "os_specific.hpp"
#include "pipe_reader.hpp"

int childPid = 0;

void disableKeyboard(void) {
    if (childPid != 0) {
        log_warn("Keyboard is already disabled");
        return;
    }

    // TODO: Make cross platform
    int pid = fork();
    if (pid == -1) {
        log_error("Failed to fork process: %s", std::strerror(errno));
        return;
    } else if (pid == 0) {
        // Child process - disable keyboard and keep process running
        blockKeys();

        // Wait until keyboard is re-enabled, without taking up CPU
        while (true) {
            sleep(1);
        }
    } else {
        // Parent process - save child pid
        childPid = pid;
    }
}

void enableKeyboard(void) {
    if (childPid == 0) {
        log_warn("Keyboard is already enabled");
        return;
    }

    // WARNING: If kill() is called with pid -1, it will send SIGTERM to all processes
    if (kill(childPid, SIGTERM) == -1) {
        log_error("Failed to kill child process: %s", std::strerror(errno));
    } else {
        childPid = 0;
        log_info("Enabled keyboard");
    }
}

void runCommand(std::string command) {
    if (command == DISABLE) {
        disableKeyboard();
    } else if (command == ENABLE) {
        enableKeyboard();
    } else {
        log_error("Unknown command: '%s'", command.c_str());
    }
}

int main() {
    log_info("Starting keyboard disable service...");

    if (std::atexit(&enableKeyboard) == -1) {
        log_error("Failed to register exit handler: %s", std::strerror(errno));
        return 1;
    }

    PipeReader pipeReader;
    if (pipeReader.open() == -1) {
        log_error("Failed to open named pipe: %s", std::strerror(errno));
        return 1;
    }
    log_debug("Opened named pipe");

    while (true) {
        std::string buf = pipeReader.read();
        if (!buf.empty()) {
            log_debug("Read from named pipe: '%s'", buf.c_str());
            runCommand(buf);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

    return 0;
}
