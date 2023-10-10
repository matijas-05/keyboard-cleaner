#ifndef TRAY_HPP
#define TRAY_HPP

#include <QtWidgets>
#include "pipe_writer.hpp"

class Tray : public QObject {
    Q_OBJECT

   public:
    Tray(PipeWriter& pipeWriter)
        : m_tray(QSystemTrayIcon()), m_trayMenu(QMenu()), m_pipeWriter(pipeWriter){};
    void init();
    void show();

   public slots:
    void toggleKeyboard(bool value) const;
    void quit() const;

   private:
    QSystemTrayIcon m_tray;
    QMenu m_trayMenu;
    PipeWriter& m_pipeWriter;
};
#endif  // !TRAY_HPP
