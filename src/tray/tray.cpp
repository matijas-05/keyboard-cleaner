#include "tray.hpp"
#include "../config.hpp"
#include "log.c/log.h"
#include "pipe_writer.hpp"

void Tray::init() {
    if (!QSystemTrayIcon::isSystemTrayAvailable()) {
        log_error("System tray is not available");
        std::exit(1);
    }

    QAction* disableKeyboard = new QAction("Disable keyboard", this);
    disableKeyboard->setCheckable(true);
    connect(disableKeyboard, &QAction::toggled, this, &Tray::toggleKeyboard);

    QAction* quit = new QAction("Quit", this);
    connect(quit, &QAction::triggered, this, &Tray::quit);

    m_trayMenu.addAction(disableKeyboard);
    m_trayMenu.addAction(quit);

    m_tray.setContextMenu(&m_trayMenu);
}
void Tray::show() {
    m_tray.show();
}

// slots
void Tray::toggleKeyboard(bool value) const {
    if (m_pipeWriter.write(value ? DISABLE : ENABLE) == -1) {
        log_error("Failed to write to named pipe: %s", std::strerror(errno));
    }
}
void Tray::quit() const {
    m_pipeWriter.write(ENABLE);
    std::exit(0);
}
