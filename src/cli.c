#include <fcntl.h>
#include <linux/input.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include "log.c/src/log.h"

#define GRAB 1
#define RELEASE 0

const char* const DEVICE_PATH = "/dev/input/event14";
bool disabled = false;

void toggle_keyboard(void) {
    int fd = open(DEVICE_PATH, O_RDWR);
    if (fd == -1) {
        log_error("Failed to open device");
        close(fd);
        return;
    }
    if (ioctl(fd, EVIOCGRAB, GRAB) == -1) {
        log_error("Failed to disable keyboard");
        close(fd);
        return;
    }

    disabled = true;
    log_info("Disabled keyboard");

    // TODO: Fork to prevent blocking the main thread
    // Kill fork when received message to enable keyboard
    while (disabled) {
    };

    close(fd);
}

void enable_keyboard(void) {
    disabled = false;
    log_info("Enabled keyboard");
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        log_error("Invalid arguments");
        return 1;
    } else {
        if (strcmp(argv[0], "disable")) {
            toggle_keyboard();
        } else if (strcmp(argv[0], "enable")) {
            enable_keyboard();
        } else {
            log_error("Invalid arguments");
            return 1;
        }
    }
    return 0;
}
