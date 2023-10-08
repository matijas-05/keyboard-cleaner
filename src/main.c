#include <stdlib.h>

#define TRAY_APPINDICATOR 1
#include "log.c/src/log.h"
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

    while (tray_loop(1) == 0) {
    }
    tray_exit();

    return 0;
}
