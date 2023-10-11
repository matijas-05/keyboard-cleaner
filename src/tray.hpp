#pragma once

#include <QtWidgets>

class Tray : public QObject {
    Q_OBJECT

   public:
    Tray() : m_tray(QSystemTrayIcon()), m_trayMenu(QMenu()){};
    void init();
    void show();

   public slots:
    void toggleKeyboard(bool value) const;
    void quit() const;

   private:
    QSystemTrayIcon m_tray;
    QMenu m_trayMenu;
};
