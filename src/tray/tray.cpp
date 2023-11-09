#include "tray.hpp"
#include "../config.hpp"
#include "command_runner.hpp"
#include "log.c/log.h"

#ifdef __linux__
#include <fcntl.h>
#include <libevdev-1.0/libevdev/libevdev.h>
#include <filesystem>
#endif  // __linux__

void Tray::init() {
    QMenu* trayMenu = new QMenu();
    QAction* disableKeyboard = new QAction("Disable keyboard", this);
    disableKeyboard->setCheckable(true);
    connect(disableKeyboard, &QAction::toggled, this, &Tray::toggleKeyboard);

#ifdef __linux__
    QMenu* keyboardsMenu = new QMenu("Select keyboard", trayMenu);
    QActionGroup* keyboardsGroup = new QActionGroup(trayMenu);
    keyboardsGroup->setExclusive(true);

    log_debug("Keyboards found:");
    struct libevdev* dev = nullptr;
    for (const auto& entry : std::filesystem::directory_iterator("/dev/input/")) {
        if (!entry.is_directory() && entry.path().string().find("event") != std::string::npos) {
            int fd = open(entry.path().c_str(), O_RDONLY | O_NONBLOCK);
            int rc = libevdev_new_from_fd(fd, &dev);
            if (rc < 0) {
                log_error("Failed to open device for libevdev: %s", std::strerror(-rc));
                continue;
            }

            if (libevdev_has_event_type(dev, EV_KEY) && libevdev_has_event_type(dev, EV_REP)) {
                const char* name = libevdev_get_name(dev);
                log_debug("- \"%s\": %s", name, entry.path().c_str());

                QAction* action = keyboardsGroup->addAction(name);
                action->setCheckable(true);
                connect(action, &QAction::triggered, this,
                        [this, entry]() { setKeyboardPath(entry.path().string()); });
            }

            close(fd);
        }
    }
    libevdev_free(dev);

    keyboardsGroup->actions().first()->trigger();
    keyboardsMenu->addActions(keyboardsGroup->actions());
    trayMenu->addMenu(keyboardsMenu);
#endif  // __linux__

    QAction* quit = new QAction("Quit", this);
    connect(quit, &QAction::triggered, this, &Tray::quit);

    trayMenu->addAction(disableKeyboard);
    trayMenu->addAction(quit);

    QIcon icon(":/resources/icon.png");
    m_tray.setIcon(icon);
    m_tray.setContextMenu(trayMenu);
}
void Tray::show() {
    m_tray.show();
}

// slots
#ifdef __linux__
void Tray::setKeyboardPath(const std::string& path) {
    log_debug("Selected keyboard: %s", path.c_str());
    m_keyboardPath = path;
}
#endif  // __linux__
void Tray::toggleKeyboard(bool value) const {
#ifdef __linux__
    if (value) {
        if (m_commandRunner.run(std::string(DISABLE + (" " + m_keyboardPath))) == -1) {
            log_error("Failed to write to named pipe: %s", std::strerror(errno));
        }
    } else {
        if (m_commandRunner.run(ENABLE) == -1) {
            log_error("Failed to write to named pipe: %s", std::strerror(errno));
        }
    }
#else
    if (m_commandRunner.run(value ? DISABLE : ENABLE) == -1) {
        log_error("Failed to write to named pipe: %s", std::strerror(errno));
    }
#endif
}
void Tray::quit() const {
    m_commandRunner.run(ENABLE);
    std::exit(0);
}
