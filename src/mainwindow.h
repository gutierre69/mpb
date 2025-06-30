#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>
#include <QIcon>
#include <QObject>
#include <QPainter>
#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>
#include <QTimer>
#include <QDirIterator>

#include <QCloseEvent>
#include <QFocusEvent>
#include <QEvent>
#include <QPaintEvent>
#include <QMouseEvent>

#include <QAudioOutput>
#include <QAudioDecoder>
#include <QAudioBuffer>
#include <QMediaPlayer>

#include "waveform.h"
#include "audioplayer.h"
#include "aboutdialog.h"

#include "taglib/tag.h"
#include "taglib/mpegfile.h"
#include "taglib/tbytevectorstream.h"
#include "taglib/id3v2tag.h"
#include "taglib/id3v2framefactory.h"
#include "taglib/fileref.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

    public:
        MainWindow(QWidget *parent = nullptr);
        ~MainWindow();

    protected:
        void closeEvent(QCloseEvent *event) override;
        void mousePressEvent(QMouseEvent *event) override;
        void mouseMoveEvent(QMouseEvent *event) override;
        void paintEvent(QPaintEvent *) override;

    public slots:
        void trayIconActivated(QSystemTrayIcon::ActivationReason reason);

    private:
        void previous();
        void next();
        void setVolume(float volume);
        void folderSearch();
        void processFolder(QString path);
        void audioInfo(QString filename );
        void currentPosition(qint64 pos);
        void playPause();
        void muteVolume();
        void showAboutDialog();

        QPoint m_dragPosition;

        Ui::MainWindow *ui;

        QSystemTrayIcon *trayIcon;
        QMenu *trayMenu;

        Waveform *waveform;

        AudioPlayer *player;

        std::vector<QString> playlist;

        QAction *restoreAction;
        QAction *playAction;
        QAction *muteAction;
        QAction *quitAction;

        QSettings *settings;

        bool Playing = false;
        bool Mute = false;

        int current = 0;
};
#endif // MAINWINDOW_H
