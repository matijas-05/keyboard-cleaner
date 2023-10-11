#include <sys/stat.h>
#include "log.c/log.h"
#include "tray.hpp"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    if (!QSystemTrayIcon::isSystemTrayAvailable()) {
        log_error("System tray is not available");
        std::exit(1);
    }

    log_info("Starting tray UI...");
    Tray tray;
    tray.init();
    tray.show();

    return app.exec();
}
