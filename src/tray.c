#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "config.h"
#include "log.c/log.h"

// TODO: Replace with QT
#define TRAY_APPINDICATOR 1
#include "tray/tray.h"

bool disabled = false;

void toggle_keyboard(struct tray_menu* element) {
    // TODO: Named pipe
    if (!disabled) {
        system("pkexec ./keyboard-cleaner-cli disable");
        element->checked = true;
    } else {
        system("pkexec ./keyboard-cleaner-cli enable");
        element->checked = false;
    }
}

void quit(void) {
    exit(0);
}

int main(void) {
    struct tray_menu menu[] = {{
                                   .text = "Disable keyboard",
                                   .cb = &toggle_keyboard,
                               },
                               {
                                   .text = "Quit",
                                   .cb = (void (*)(struct tray_menu*))(&quit),
                               },
                               {.text = NULL}};
    struct tray tray = {.menu = menu, .icon = ""};

    log_info("Starting tray...");
    if (tray_init(&tray) == -1) {
        log_error("Failed to create tray icon");
        return 1;
    }

    if (mkfifo(FIFO_PATH, 0666) == -1 && errno != EEXIST) {
        log_error("Failed to create named pipe: %s", strerror(errno));
        return 1;
    }
    if (errno == 0) {
        log_debug("Created named pipe");
    }

    int fd = open(FIFO_PATH, O_WRONLY);
    if (fd == -1) {
        log_error("Failed to open named pipe: %s", strerror(errno));
    }
    log_debug("Opened named pipe");

    while (tray_loop(1) == 0) {
    }
    tray_exit();

    return 0;
}
