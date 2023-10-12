#include <fcntl.h>
#include <poll.h>
#include <signal.h>
#include <unistd.h>
#include <cstring>
#include <fstream>
#include <thread>
#include "../config.hpp"
#include "log.c/log.h"
#include "os_specific.hpp"

int child_pid = 0;

void disableKeyboard() {
    if (child_pid != 0) {
        log_warn("Keyboard is already disabled");
        return;
    }

    // TODO: Use threads
    int pid = fork();
    if (pid == -1) {
        log_error("Failed to fork process: %s", std::strerror(errno));
        return;
    } else if (pid == 0) {
        // Child process - disable keyboard and keep process running
        blockKeys();
    } else {
        // Parent process - save child pid
        child_pid = pid;
    }
}

void enableKeyboard() {
    if (child_pid == 0) {
        log_warn("Keyboard is already enabled");
        return;
    }

    // WARNING: If kill() is called with pid -1, it will send SIGTERM to all processes
    if (kill(child_pid, SIGTERM) == -1) {
        log_error("Failed to kill child process: %s", std::strerror(errno));
    } else {
        child_pid = 0;
        log_info("Enabled keyboard");
    }
}

void runCommand(char buf[CMD_LEN]) {
    if (strcmp(buf, DISABLE) == 0) {
        disableKeyboard();
    } else if (strcmp(buf, ENABLE) == 0) {
        enableKeyboard();
    } else {
        log_error("Unknown command: '%s'", buf);
    }
}

int main() {
    log_info("Starting keyboard disable service...");

    if (std::atexit(&enableKeyboard) == -1) {
        log_error("Failed to register exit handler: %s", std::strerror(errno));
        return 1;
    }

    std::ifstream pipe_stream(PIPE_PATH);
    if (!pipe_stream.is_open() && errno != ENOENT) {
        log_error("Failed to open named pipe: %s", std::strerror(errno));
        return 1;
    }

    // Wait until tray creates named pipe
    // Tray has to do it because it has lower permissions
    if (errno == ENOENT) {
        log_debug("Waiting for tray to create named pipe...");
    }
    while (errno == ENOENT) {
        pipe_stream.open(PIPE_PATH);
        if (pipe_stream.is_open()) {
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    log_debug("Opened named pipe");
    pipe_stream.close();

    while (true) {
        // We have to open and close the pipe every time because when tray quits, it all breaks
        char buf[CMD_LEN + 1];
        pipe_stream.open(PIPE_PATH);
        pipe_stream.read(buf, CMD_LEN);
        if (strcmp(buf, "") != 0) {
            log_debug("Read from named pipe: '%s'", buf);
            runCommand(buf);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        pipe_stream.close();
    }

    return 0;
}
