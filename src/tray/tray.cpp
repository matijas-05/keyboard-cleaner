#include "tray.hpp"
#include <fcntl.h>
#include <unistd.h>
#include <QtWidgets>
#include "config.h"
#include "log.c/log.h"

Tray::Tray() {
    if (!QSystemTrayIcon::isSystemTrayAvailable()) {
        log_error("System tray is not available");
        exit(1);
    }

    m_tray = new QSystemTrayIcon();
    m_trayMenu = new QMenu();

    QAction* disableKeyboard = new QAction("Disable keyboard", this);
    disableKeyboard->setCheckable(true);
    connect(disableKeyboard, &QAction::toggled, this, &Tray::toggleKeyboard);

    QAction* quit = new QAction("Quit", this);
    connect(quit, &QAction::triggered, this, &Tray::quit);

    m_trayMenu->addAction(disableKeyboard);
    m_trayMenu->addAction(quit);

    m_tray->setContextMenu(m_trayMenu);
}
void Tray::show() const {
    m_tray->show();
}

int pipe_fd = -1;
int send_command(char* command) {
    assert(strlen(command) == CMD_LEN);

    if (write(pipe_fd, command, strlen(command)) == -1) {
        log_error("Failed to write to named pipe: %s", strerror(errno));
        return -1;
    }
    return 0;
}

void Tray::toggleKeyboard(bool value) const {
    send_command(value ? (char*)DISABLE : (char*)ENABLE);
}
void Tray::quit() const {
    send_command((char*)ENABLE);
    exit(0);
}

int main(int argc, char* argv[]) {
    // if (atexit(&Tray::quit) == -1) {
    //     log_error("Failed to register exit handler: %s", strerror(errno));
    //     return 1;
    // }

    QApplication app(argc, argv);

    Tray* tray = new Tray();
    tray->show();

    log_info("Waiting for keyboard disable service to start...");
    pipe_fd = open(PIPE_PATH, O_WRONLY);
    if (pipe_fd == -1) {
        log_error("Failed to open named pipe: %s", strerror(errno));
    }
    log_debug("Opened named pipe");

    log_info("Starting tray UI...");
    return app.exec();
}
