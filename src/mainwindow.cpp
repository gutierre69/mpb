#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QPainter>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setFixedSize(400, 240);
    this->window()->setWindowTitle("MPB");
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::Window);
    this->setAttribute(Qt::WA_TranslucentBackground);

    player = new AudioPlayer;

    settings = new QSettings("MPB", "config", this);
    if(!settings->contains("interface/volume"))
        settings->setValue("interface/volume", player->getVolume());

    waveform = new Waveform(this);
    waveform->setGeometry(50, 40, 300, 60);
    waveform->setStyleSheet("background-color: transparent;");
    waveform->raise();
    waveform->setAttribute(Qt::WA_TransparentForMouseEvents, false);
    waveform->setAttribute(Qt::WA_OpaquePaintEvent);
    waveform->show();
    waveform->empty();

    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setIcon(QIcon(":/images/icons/icons8-play-64.png"));
    trayIcon->setToolTip("TocaE");

    trayMenu = new QMenu(this);
    QAction *restoreAction = new QAction(tr("Restaurar"), this);
    QAction *quitAction = new QAction(tr("Sair"), this);

    trayMenu->addAction(restoreAction);
    trayMenu->addAction(quitAction);
    trayIcon->setContextMenu(trayMenu);
    trayIcon->show();

    ui->btnClose->move(10,10);
    ui->btnClose->setText("");
    ui->btnClose->setFixedSize(25, 25);
    ui->btnClose->setIcon(QIcon(":/images/icons/icons8-close-64.png"));
    ui->btnClose->setIconSize(QSize(10, 10));

    ui->btnPlay->move(175,180);
    ui->btnPlay->setText("");
    ui->btnPlay->setFixedSize(50, 50);
    ui->btnPlay->setIcon(QIcon(":/images/icons/icons8-play-64.png"));
    ui->btnPlay->setIconSize(QSize(15, 15));

    ui->btnPrevious->move(125,185);
    ui->btnPrevious->setText("");
    ui->btnPrevious->setFixedSize(40, 40);
    ui->btnPrevious->setIcon(QIcon(":/images/icons/icons8-go-to-start-64.png"));
    ui->btnPrevious->setIconSize(QSize(15, 15));

    ui->btnNext->move(235,185);
    ui->btnNext->setText("");
    ui->btnNext->setFixedSize(40, 40);
    ui->btnNext->setIcon(QIcon(":/images/icons/icons8-go-to-end-64.png"));
    ui->btnNext->setIconSize(QSize(15, 15));

    ui->btnMenu->move(350,185);
    ui->btnMenu->setText("");
    ui->btnMenu->setFixedSize(40, 40);
    ui->btnMenu->setIcon(QIcon(":/images/icons/icons8-folder-64.png"));
    ui->btnMenu->setIconSize(QSize(15, 15));

    ui->btnMute->move(17,200);
    ui->btnMute->setText("");
    ui->btnMute->setFixedSize(25, 25);
    ui->btnMute->setIcon(QIcon(":/images/icons/icons8-mute-64.png"));
    ui->btnMute->setIconSize(QSize(15, 15));

    ui->btnAbout->move(358,150);
    ui->btnAbout->setText("");
    ui->btnAbout->setFixedSize(25, 25);
    ui->btnAbout->setIcon(QIcon(":/images/icons/icons8-help-64.png"));
    ui->btnAbout->setIconSize(QSize(15, 15));

    ui->volume->move(20,105);
    ui->currentTime->move(50,100);
    ui->remainTime->move(285,100);
    ui->music_title->move(50,125);
    ui->artist_name->move(50,145);

    player->setVolume( settings->value("interface/volume").toFloat() );
    ui->volume->setValue( settings->value("interface/volume").toFloat() * 100 );
    connect(ui->volume, &QSlider::sliderMoved, this, &MainWindow::setVolume);

    connect(ui->btnClose, &QPushButton::clicked, this, &QMainWindow::close);
    connect(ui->btnPlay, &QPushButton::clicked, this, &MainWindow::playPause);
    connect(ui->btnMute, &QPushButton::clicked, this, &MainWindow::muteVolume);
    connect(ui->btnMenu, &QPushButton::clicked, this, &MainWindow::folderSearch);
    connect(ui->btnPrevious, &QPushButton::clicked, this, &MainWindow::previous);
    connect(ui->btnNext, &QPushButton::clicked, this, &MainWindow::next);
    connect(ui->btnAbout, &QPushButton::clicked, this, &MainWindow::showAboutDialog);

    connect(player, &AudioPlayer::loadedMedia, this, &MainWindow::audioInfo);
    connect(player, &AudioPlayer::haveWaveform, waveform, &Waveform::setWaveform);
    connect(player, &AudioPlayer::update_position, this, &MainWindow::currentPosition);
    connect(player, &AudioPlayer::statusChanged, this, [=](QMediaPlayer::MediaStatus state) {
        if(state==QMediaPlayer::EndOfMedia && Playing){
            next();
        }
    });
    connect(waveform, &Waveform::seekRequested, this, [=](qint64 pos) {
        player->Seek(pos);
    });

    connect(restoreAction, &QAction::triggered, this, [=](){
        showNormal();
        activateWindow();
        this->raise();
        this->activateWindow();
    });
    connect(quitAction, &QAction::triggered, qApp, &QCoreApplication::quit);
    connect(trayIcon, &QSystemTrayIcon::activated, this, &MainWindow::trayIconActivated);

    processFolder( settings->value("interface/folder").toString() );
    current = settings->value("interface/current",0).toInt();

    if(playlist.size()>0 && (current>=0 && current<playlist.size())){
        QString filename = playlist.at(current);
        player->addMedia( filename );
    }
}

MainWindow::~MainWindow()
{
    player->Stop();

    delete trayIcon;
    delete trayMenu;
    delete waveform;
    delete player;
    delete ui;
}

void MainWindow::muteVolume()
{
    if(Mute){
        player->setVolume( settings->value("interface/volume").toFloat() );
        ui->volume->setEnabled(true);
        ui->volume->setValue( player->getVolume() * 100 );
        ui->btnMute->setStyleSheet("");
        Mute = false;
    } else if(!Mute){
        player->setVolume( 0 );
        ui->volume->setEnabled(false);
        ui->volume->setValue( player->getVolume() * 100 );
        ui->btnMute->setStyleSheet("background-color: rgba(255,0,0,0.2);");
        Mute = true;
    }
}

void MainWindow::playPause()
{
    if(!player->ready)
        return;

    if(player->isPlaying()) {
        ui->btnPlay->setIcon(QIcon(":/images/icons/icons8-play-64.png"));
        player->Pause();
        Playing = false;
    } else {
        ui->btnPlay->setIcon(QIcon(":/images/icons/icons8-pause-64.png"));
        player->Play();
        Playing = true;
    }
}


void MainWindow::currentPosition(qint64 pos)
{
    waveform->setPlaybackPosition( player->getPosition() );
    waveform->setTotalDuration(  player->getDuration() );

    if(!Playing)
        return;

    qint64 remainingDuration = player->remainingTime();

    qint64 seconds = (remainingDuration / 1000) % 60;
    qint64 minutes = (remainingDuration / 1000) / 60;

    QString remainTime = QString("%1:%2").arg(minutes, 2, 10, QChar('0')).arg(seconds, 2, 10, QChar('0'));
    ui->remainTime->setText( remainTime );

    qint64 currentPosition = player->getPosition();

    qint64 current_seconds = (currentPosition / 1000) % 60;
    qint64 current_minutes = (currentPosition / 1000) / 60;

    QString currentTime = QString("%1:%2").arg(current_minutes, 2, 10, QChar('0')).arg(current_seconds, 2, 10, QChar('0'));
    ui->currentTime->setText( currentTime );
}

void MainWindow::audioInfo(QString filename )
{
    QString title = tr("Desconhecido");
    QString name = tr("Desconhecido");

    TagLib::FileRef aud(filename.toStdString().c_str());
    if (!aud.isNull() && aud.audioProperties()) {
        int totalSeconds = aud.audioProperties()->length();
        int minutes = totalSeconds / 60;
        int seconds = totalSeconds % 60;

        ui->currentTime->setText( "00:00" );

        QString duration = QString("%1:%2").arg(minutes, 2, 10, QChar('0')).arg(seconds, 2, 10, QChar('0'));
        ui->remainTime->setText( duration );

        if(aud.tag()->title()!="")
            title = aud.tag()->title().toCString(true);

        if(aud.tag()->artist()!="")
            name = aud.tag()->artist().toCString(true);
    }

    ui->music_title->setText( title );
    ui->artist_name->setText( name );
}

void MainWindow::folderSearch()
{
    player->Stop();
    Playing = false;
    ui->btnPlay->setIcon(QIcon(":/images/icons/icons8-play-64.png"));

    QString directory = settings->value("interface/folder", QDir::homePath()).toString();

    QString path = QFileDialog::getExistingDirectory(this, tr("Selecionar pasta de músicas"), directory);
    if (!path.isEmpty()) {
        settings->setValue("interface/folder", path);
        player->Reset();
        processFolder( path );

    }
}

void MainWindow::processFolder(QString path)
{
    if(path.isEmpty())
        return;

    playlist.clear();
    current = 0;
    waveform->empty();
    player->addMedia( "" );
    ui->music_title->setText( "" );
    ui->artist_name->setText( "" );

    QStringList filters = { "*.mp3", "*.wav", "*.ogg", "*.flac", "*.mp4", "*.wma" };
    QStringList audioFiles;

    QDirIterator it(path, filters, QDir::Files, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        audioFiles << it.next();
    }

    if (!audioFiles.isEmpty()) {

        for(int i=0; i<audioFiles.size(); i++){
            QString selectedFile = audioFiles.at(i);
            playlist.push_back({ selectedFile });
        }

        player->addMedia( playlist[ current ] );
    }
}

void MainWindow::setVolume(float volume)
{
    player->setVolume( volume / 100 );
    settings->setValue("interface/volume", volume/100);
}

void MainWindow::previous()
{
    if(playlist.size()==0)
        return;

    if(Playing) player->Stop();

    current -= 1;
    if(current<0)
        current = playlist.size()-1;

    player->addMedia( playlist.at(current) );
    if(Playing) player->Play();

    settings->setValue("interface/current", current);
}

void MainWindow::next()
{
    if(playlist.size()==0)
        return;

    if(Playing) player->Stop();

    current += 1;
    if(current>playlist.size()-1)
        current = 0;

    player->addMedia( playlist.at(current)  );
    if(Playing) player->Play();

    settings->setValue("interface/current", current);
}

void MainWindow::closeEvent(QCloseEvent *event) {
    //QCoreApplication::quit();
    if (trayIcon->isVisible()) {
        hide();
        event->ignore();

        trayIcon->showMessage(tr("Minimizado"), tr("Clique 2x para restaurar"), QSystemTrayIcon::Information, 0);
    } else {
        event->accept();
    }
}

void MainWindow::trayIconActivated(QSystemTrayIcon::ActivationReason reason) {
    if (reason == QSystemTrayIcon::Trigger || reason == QSystemTrayIcon::DoubleClick) {
        showNormal();
        activateWindow();
        this->raise();
        this->activateWindow();
    }
}


void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_dragPosition = event->globalPos() - frameGeometry().topLeft();
        event->accept();
    }
    QWidget::mousePressEvent(event);
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton) {
        move((event->globalPosition() - m_dragPosition).toPoint());
        event->accept();
    }
    QWidget::mouseMoveEvent(event);
}

void MainWindow::showAboutDialog()
{
    AboutDialog aboutDialog;
    aboutDialog.setWindowTitle(tr("Sobre o MPB"));

    QRect parentRect = this->geometry();
    int x = parentRect.center().x() - aboutDialog.width() / 2;
    int y = parentRect.center().y() - aboutDialog.height() / 2;
    aboutDialog.move(x, y);

    aboutDialog.exec();
}

void MainWindow::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    QBrush brush(QColor("#2d283c"));
    p.setBrush(brush);
    p.setPen(Qt::NoPen);

    QRect rect = this->rect();
    p.drawRoundedRect(rect, 15, 15);
}

