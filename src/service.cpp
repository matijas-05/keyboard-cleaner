#include "service.hpp"
#include <signal.h>
#include <unistd.h>
#include <cerrno>
#include <cstring>
#include "log.c/log.h"
#include "os_specific.hpp"

int child_pid = 0;

void disableKeyboard(void) {
    if (child_pid != 0) {
        log_warn("Keyboard is already disabled");
        return;
    }

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
        child_pid = pid;
    }
}

void enableKeyboard(void) {
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
