#include <qapplication.h>
#include <cstdlib>
#include "log.c/log.h"
#include "tray.hpp"

Tray* tray;

void onExit() {
    tray->quit();
}

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    PipeWriter pipeWriter;

    tray = new Tray(&pipeWriter);
    tray->show();

    if (atexit(&onExit) == -1) {
        log_error("Failed to register exit handler: %s", strerror(errno));
        return 1;
    }

    log_info("Waiting for keyboard disable service to start...");
    pipeWriter.open();
    log_debug("Opened named pipe");

    log_info("Starting tray UI...");
    return app.exec();
}
