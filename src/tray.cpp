#include "tray.hpp"
#include "log.c/log.h"
#include "service.hpp"

void Tray::init() {
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
    if (value) {
        disableKeyboard();
    } else {
        enableKeyboard();
    }
}
void Tray::quit() const {
    std::exit(0);
}
