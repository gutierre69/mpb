#include "mainwindow.h"

#include <QApplication>
#include <QScreen>
#include <QFile>
#include <QSharedMemory>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QSharedMemory sharedMemory;
    sharedMemory.setKey("MBP-gutierre69-music-player-brazil-2025");

    if (!sharedMemory.create(1)) {
        return 0;
    }

    QCoreApplication::setOrganizationName("MPB");
    QCoreApplication::setApplicationName("mpb");

    QFile styleFile(":/images/style.qss");

    if (styleFile.open(QFile::ReadOnly | QFile::Text)) {
        QString styleSheet = styleFile.readAll();
        a.setStyleSheet(styleSheet);
        styleFile.close();
    } else {
        qWarning("Não carregou estilos (style.qss)");
    }

    MainWindow w;
    // QScreen *screen = QApplication::primaryScreen();
    // if (screen) {
    //     QRect screenRect = screen->geometry();

    //     int x = (screenRect.right()/2) - (w.width()/2);
    //     int y = screenRect.top()+50;
    //     w.move(x, y);
    // }

    w.show();
    return a.exec();
}
