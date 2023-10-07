#define TRAY_APPINDICATOR 1
#include "log.c/src/log.h"
#include "tray/tray.h"

int main(void) {
    struct tray_menu menu[] = {
        {
            .text = "Keyboard Cleaner",
            .disabled = 1,
        },
        {
            .text = "Disable keyboard",
        },
    };
    struct tray tray = {.menu = menu, .icon = ""};

    log_info("Starting tray...");
    tray_init(&tray);
    while (tray_loop(1) == 0) {
    }
    tray_exit();

    return 0;
}
