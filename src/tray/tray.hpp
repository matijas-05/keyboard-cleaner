#pragma once

#include <QtWidgets>
#include "pipe_writer.hpp"

#ifdef __linux__
#include <string>
#endif  // __linux__

class Tray : public QObject {
    Q_OBJECT

   public:
    Tray(PipeWriter& pipeWriter) : m_tray(QSystemTrayIcon()), m_pipeWriter(pipeWriter){};
    void init();
    void show();

   public slots:
#ifdef __linux__
    void setKeyboardPath(const std::string& path);
#endif  // __linux__
    void toggleKeyboard(bool value) const;
    void quit() const;

   private:
    QSystemTrayIcon m_tray;
    PipeWriter& m_pipeWriter;
#ifdef __linux__
    std::string m_keyboardPath;
#endif  // __linux__
};
