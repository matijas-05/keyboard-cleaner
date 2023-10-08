#include <assert.h>
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

int pipe_fd = -1;
struct tray tray;

int send_command(char* command) {
    assert(strlen(command) == CMD_LEN);

    if (write(pipe_fd, command, strlen(command)) == -1) {
        log_error("Failed to write to named pipe: %s", strerror(errno));
        return -1;
    }
    return 0;
}

void toggle_keyboard(struct tray_menu* menuitem) {
    if (!menuitem->checked) {
        if (send_command(DISABLE) != -1) {
            menuitem->checked = true;
        }
    } else {
        if (send_command(ENABLE) != -1) {
            menuitem->checked = false;
        }
    }
    tray_update(&tray);
}

void quit(void) {
    send_command(ENABLE);
    exit(0);
}

int main(void) {
    if (atexit(&quit) == -1) {
        log_error("Failed to register exit handler: %s", strerror(errno));
        return 1;
    }

    struct tray_menu menu[] = {{
                                   .text = "Disable keyboard",
                                   .cb = &toggle_keyboard,
                               },
                               {
                                   .text = "Quit",
                                   .cb = (void (*)(struct tray_menu*))(&quit),
                               },
                               {.text = NULL}};
    tray = (struct tray){.menu = menu, .icon = "icon.png"};

    log_info("Waiting for keyboard disable service to start...");

    if (mkfifo(PIPE_PATH, 0666) == -1 && errno != EEXIST) {
        log_error("Failed to create named pipe: %s", strerror(errno));
        return 1;
    }
    if (errno == 0) {
        log_debug("Created named pipe");
    }

    pipe_fd = open(PIPE_PATH, O_WRONLY);
    if (pipe_fd == -1) {
        log_error("Failed to open named pipe: %s", strerror(errno));
    }
    log_debug("Opened named pipe");

    log_info("Starting tray UI...");
    if (tray_init(&tray) == -1) {
        log_error("Failed to create tray icon");
        return 1;
    }
    while (tray_loop(1) == 0) {
    }

    tray_exit();

    return 0;
}
