#ifndef AUDIOPLAYER_H
#define AUDIOPLAYER_H

#include <QAudioOutput>
#include <QAudioDecoder>
#include <QAudioBuffer>
#include <QMediaPlayer>

#include <QObject>
#include <QDebug>

class AudioPlayer : public QMediaPlayer
{
    Q_OBJECT

    public:
        float volume = 0.5f;
        bool ready = false;

        AudioPlayer();
        void Reset();
        void Play();
        void Pause();
        void Stop();
        void addMedia(QString file);
        void Seek(int mseconds);
        qint64 getPosition();
        qint64 getDuration();
        qint64 remainingTime();
        bool isPlaying();
        bool isPaused();
        bool isStopped();
        qreal getVolume();
        void setVolume(float volume);

        void audioDecoder(QString filename);

    signals:
        void update_position( qint64 position );
        void haveWaveform( QVector<float> waveform );
        void loadedMedia(QString filename );
        void statusChanged(QMediaPlayer::MediaStatus state);

    private slots:
        void positionChanged(qint64 position );

    private:
        int current_length;

        QMediaPlayer *player;
        QAudioOutput *audioOutput;
        QAudioDecoder *decoder;

        QVector<float> waveform;
};

#endif // AUDIOPLAYER_H
