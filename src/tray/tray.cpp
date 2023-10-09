#include "tray.hpp"
#include <fcntl.h>
#include <unistd.h>
#include <QtWidgets>
#include "config.h"
#include "log.c/log.h"
#include "pipe-writer.hpp"

Tray::Tray(PipeWriter* pipeWriter) {
    if (!QSystemTrayIcon::isSystemTrayAvailable()) {
        log_error("System tray is not available");
        exit(1);
    }

    m_pipeWriter = pipeWriter;

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

// slots
void Tray::toggleKeyboard(bool value) const {
    if (m_pipeWriter->write(value ? DISABLE : ENABLE) == -1) {
        log_error("Failed to write to named pipe: %s", strerror(errno));
    }
}
void Tray::quit() const {
    m_pipeWriter->write(ENABLE);
    exit(0);
}
