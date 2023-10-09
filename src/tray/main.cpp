#include <qapplication.h>
#include <cstdlib>
#include "log.c/log.h"
#include "tray.hpp"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    PipeWriter pipeWriter;

    Tray tray(pipeWriter);
    tray.init();
    tray.show();

    log_info("Waiting for keyboard disable service to start...");
    pipeWriter.open();
    log_debug("Opened named pipe");

    log_info("Starting tray UI...");
    return app.exec();
}
