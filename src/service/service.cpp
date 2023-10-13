#include <cstring>
#include <thread>
#include "../config.hpp"
#include "log.c/log.h"
#include "os_specific.hpp"
#include "pipe_reader.hpp"

#if defined(__linux__) || defined(__APPLE__)
#include <signal.h>
#include <unistd.h>
int childPid = 0;

void disableKeyboard() {
    if (childPid != 0) {
        log_warn("Keyboard is already disabled");
        return;
    }

    // Fork on linux and mac, because it's the easiest way to do it on mac,
    // and it can also be done this way on linux, even though it's not necessary
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
void enableKeyboard() {
    if (childPid == 0) {
        log_warn("Keyboard is already enabled");
        return;
    }

    if (kill(childPid, SIGTERM) == -1) {
        log_error("Failed to kill child process: %s", std::strerror(errno));
    } else {
        childPid = 0;
        log_info("Enabled keyboard");
    }
}
#elif defined(_WIN32)
// TODO: Implement https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-blockinput
#endif

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
        // For when the pipe is closed from the tray's side
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

    return 0;
}
