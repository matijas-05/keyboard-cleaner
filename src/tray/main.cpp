#include <sys/stat.h>
#include "command_runner.hpp"
#ifdef __APPLE__
#include "../config.hpp"
#endif
#include "log.c/log.h"
#include "tray.hpp"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    CommandRunner commandRunner;

    if (!QSystemTrayIcon::isSystemTrayAvailable()) {
        log_error("System tray is not available");
        std::exit(1);
    }

#ifdef __APPLE__
    if (mkfifo(PIPE_PATH, 0666) == -1 && errno != EEXIST) {
        log_error("Failed to create named pipe: %s", std::strerror(errno));
        std::exit(1);
    } else {
        log_debug("Created named pipe");
    }
#endif

#ifdef __APPLE__
    log_info("Waiting for keyboard disable service to start...");
    if (commandRunner.open() == -1) {
        log_error("Failed to open named pipe: %s", std::strerror(errno));
        std::exit(1);
    } else {
        log_debug("Opened named pipe");
    }
#endif

    log_info("Starting tray UI...");
    Tray tray(commandRunner);
    tray.init();
    tray.show();

    return app.exec();
}
