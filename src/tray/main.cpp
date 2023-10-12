#include <sys/stat.h>
#include "../config.hpp"
#include "log.c/log.h"
#include "tray.hpp"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    PipeWriter pipeWriter;

    if (!QSystemTrayIcon::isSystemTrayAvailable()) {
        log_error("System tray is not available");
        std::exit(1);
    }

    // TODO: Make cross platform: fifos on unix and unnamed pipes on windows
    // https://learn.microsoft.com/en-us/windows/win32/ipc/anonymous-pipe-operations
    // https://stackoverflow.com/questions/26561604/create-windows-named-pipe-in-c/26561999#26561999
    if (mkfifo(PIPE_PATH, 0666) == -1 && errno != EEXIST) {
        log_error("Failed to create named pipe: %s", std::strerror(errno));
        std::exit(1);
    } else {
        log_debug("Created named pipe");
    }

    log_info("Waiting for keyboard disable service to start...");
    if (pipeWriter.open() == -1) {
        log_error("Failed to open named pipe: %s", std::strerror(errno));
        std::exit(1);
    } else {
        log_debug("Opened named pipe");
    }

    log_info("Starting tray UI...");
    Tray tray(pipeWriter);
    tray.init();
    tray.show();

    return app.exec();
}
