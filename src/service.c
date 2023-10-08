#include <errno.h>
#include <fcntl.h>
#include <linux/input.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include "config.h"
#include "log.c/log.h"

#define DEVICE_PATH "/dev/input/event14"

int child_pid = -1;

void disable_keyboard(void) {
    if (child_pid != -1) {
        log_error("Keyboard is already disabled");
        return;
    }

    // TODO: Communicate via pipe with parent process to send errors
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

        // TODO: Remove when named pipe is done
        sleep(3);
        exit(0);

        while (true) {
        }
    } else {
        // Parent process - save child pid
        child_pid = pid;
    }
}

int main(void) {
    if (getuid() != 0) {
        log_error("This program must be run as root");
        return 1;
    }
    log_info("Starting keyboard disable service...");

    int fd = open(FIFO_PATH, O_RDONLY);
    if (fd == -1 && errno != ENOENT) {
        log_error("Failed to open named pipe: %s", strerror(errno));
        return 1;
    }

    // Wait until tray creates named pipe
    // Tray has to do it because it has lower permissions
    if (errno == ENOENT) {
        log_debug("Waiting for tray to create named pipe...");
    }
    while (errno == ENOENT) {
        fd = open(FIFO_PATH, O_RDONLY);
        if (fd != -1) {
            break;
        }
    }
    log_debug("Opened named pipe");

    while (true) {
    }

    return 0;
}
