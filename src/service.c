#include <errno.h>
#include <fcntl.h>
#include <linux/input.h>
#include <poll.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include "config.h"
#include "log.c/log.h"

// TODO: Select device
#define DEVICE_PATH "/dev/input/event14"

int child_pid = 0;

void disable_keyboard(void) {
    if (child_pid != 0) {
        log_warn("Keyboard is already disabled");
        return;
    }

    int pid = fork();
    if (pid == -1) {
        log_error("Failed to fork process: %s", strerror(errno));
        return;
    } else if (pid == 0) {
        // Child process - disable keyboard and keep process running

        // Wait 100ms to avoid stuck key
        nanosleep(&(struct timespec){.tv_nsec = 100 * 1000 * 1000}, NULL);

        int fd = open(DEVICE_PATH, O_WRONLY);
        if (fd == -1) {
            log_error("Failed to open device: %s", strerror(errno));
            close(fd);
            return;
        }
        if (ioctl(fd, EVIOCGRAB, 1) == -1) {
            log_error("Failed to disable keyboard: %s", strerror(errno));
            close(fd);
            return;
        }
        log_info("Disabled keyboard");

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
        log_error("Failed to kill child process: %s", strerror(errno));
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

int main(void) {
    if (getuid() != 0) {
        log_error("This program must be run as root");
        return 1;
    }
    log_info("Starting keyboard disable service...");

    if (atexit(&enable_keyboard) == -1) {
        log_error("Failed to register exit handler: %s", strerror(errno));
        return 1;
    }

    int pipe_fd = open(PIPE_PATH, O_RDONLY);
    if (pipe_fd == -1 && errno != ENOENT) {
        log_error("Failed to open named pipe: %s", strerror(errno));
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
            log_error("Error polling named pipe: %s", strerror(errno));
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
