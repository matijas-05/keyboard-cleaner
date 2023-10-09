#include <QtWidgets>
#include "pipe-writer.hpp"

class Tray : public QObject {
    Q_OBJECT

   public:
    Tray(PipeWriter* pipeWriter);
    void show() const;

   public slots:
    void toggleKeyboard(bool value) const;
    void quit() const;

   private:
    QSystemTrayIcon* m_tray;
    QMenu* m_trayMenu;
    PipeWriter* m_pipeWriter;
};
