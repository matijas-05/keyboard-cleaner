#include <fcntl.h>
#include <poll.h>
#include <signal.h>
#include <unistd.h>
#include <cstring>
#include <thread>
#include "../config.hpp"
#include "log.c/log.h"
#include "os_specific.hpp"

int child_pid = 0;

void disable_keyboard(void) {
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

void enable_keyboard(void) {
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

void run_command(char buf[CMD_LEN]) {
    if (strcmp(buf, DISABLE) == 0) {
        disable_keyboard();
    } else if (strcmp(buf, ENABLE) == 0) {
        enable_keyboard();
    } else {
        log_error("Unknown command: '%s'", buf);
    }
}

int main() {
    // TODO: Make cross-platform (root not required on macos)
    if (getuid() != 0) {
        log_error("This program must be run as root");
        return 1;
    }
    log_info("Starting keyboard disable service...");

    if (std::atexit(&enable_keyboard) == -1) {
        log_error("Failed to register exit handler: %s", std::strerror(errno));
        return 1;
    }

    int pipe_fd = open(PIPE_PATH, O_RDONLY);
    if (pipe_fd == -1 && errno != ENOENT) {
        log_error("Failed to open named pipe: %s", std::strerror(errno));
        return 1;
    }

    // Wait until tray creates named pipe
    // Tray has to do it because it has lower permissions
    if (errno == ENOENT) {
        log_debug("Waiting for tray to create named pipe...");
    }
    while (errno == ENOENT) {
        pipe_fd = open(PIPE_PATH, O_RDONLY);
        if (pipe_fd != -1) {
            break;
        }
    }
    log_debug("Opened named pipe");

    struct pollfd pfds[1] = {{
        .fd = pipe_fd,
        .events = POLLIN,
    }};
    while (true) {
        if (poll(pfds, 1, -1) == -1) {
            log_error("Error polling named pipe: %s", std::strerror(errno));
        } else if (pfds[0].revents & POLLIN) {
            char buf[CMD_LEN + 1];
            read(pipe_fd, &buf, CMD_LEN / sizeof(char));
            buf[1] = '\0';

            log_debug("Read from named pipe: '%s'", buf);
            run_command(buf);
        } else {
            // Avoid high CPU usage when polling fails (eg. when tray is disconnected)
            sleep(1);
        }
    }

    return 0;
}
