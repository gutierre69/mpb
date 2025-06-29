#include "audioplayer.h"



AudioPlayer::AudioPlayer()
{
    current_length = 0;
    player = new QMediaPlayer;
    audioOutput = new QAudioOutput;

    decoder = new QAudioDecoder;

    player->setAudioOutput(audioOutput);
    audioOutput->setVolume( volume );

    //connect(player, SIGNAL(currentIndexChanged(int)), this, SLOT(currentIndexChanged(int)));
    connect(player, &QMediaPlayer::positionChanged, this, &AudioPlayer::positionChanged);

    // -------

    connect(decoder, &QAudioDecoder::bufferReady, this, [=]() {
        QAudioBuffer buffer = decoder->read();
        QAudioFormat format = buffer.format();

        const qint16 *data = buffer.constData<qint16>();
        int channels = format.channelCount();
        int samples = buffer.sampleCount();

        if (format.sampleFormat() == QAudioFormat::Int16) {
            const qint16 *data = buffer.constData<qint16>();
            for (int i = 0; i < samples; i += channels) {
                float sum = 0.0f;
                for (int c = 0; c < channels; ++c)
                    sum += std::abs(data[i + c]) / 32768.0f;  // normaliza Int16
                waveform.append(sum / channels);
            }
        }
        else if (format.sampleFormat() == QAudioFormat::Float) {
            const float *data = buffer.constData<float>();
            for (int i = 0; i < samples; i += channels) {
                float sum = 0.0f;
                for (int c = 0; c < channels; ++c)
                    sum += std::abs(data[i + c]);  // já está entre -1.0 e 1.0
                waveform.append(sum / channels);
            }
        }
        else {
            qWarning() << "Formato de áudio não suportado:" << format.sampleFormat();
        }
    });

    connect(decoder, &QAudioDecoder::finished, this, [=]() {
        emit haveWaveform( waveform );
    });


    connect(player, &QMediaPlayer::mediaStatusChanged, this, [=](QMediaPlayer::MediaStatus state) {
        emit statusChanged( state );
    });

}

void AudioPlayer::Reset()
{
    player->stop();
    decoder->stop();
    waveform.clear();
    for(int i=0; i<100; i++){
        waveform.push_back({0});
    }
    current_length = 0;
}

void AudioPlayer::Play()
{
    player->play();
}

void AudioPlayer::Pause()
{
    player->pause();
}

void AudioPlayer::Stop()
{
    player->stop();
}

void AudioPlayer::positionChanged( qint64 position )
{
    emit update_position( position );
}

void AudioPlayer::Seek(int mseconds)
{
    player->setPosition(mseconds);
}

void AudioPlayer::addMedia(QString file)
{
    Reset();
    player->setSource(QUrl::fromLocalFile( file ));
    decoder->setSource(QUrl::fromLocalFile( file ));
    decoder->start();
    emit loadedMedia( file );
    emit haveWaveform( waveform );
    ready=true;
}

qint64 AudioPlayer::getDuration()
{
    return player->duration();
}

qint64 AudioPlayer::getPosition()
{
    return player->position();
}

qint64 AudioPlayer::remainingTime()
{
    qint64 remainingDuration = getDuration() - getPosition();
    return remainingDuration;
}

bool AudioPlayer::isPlaying()
{
    if(player->playbackState()==QMediaPlayer::PlayingState) return true;
    return false;
}

bool AudioPlayer::isPaused()
{
    if(player->playbackState()==QMediaPlayer::PausedState) return true;
    return false;
}

bool AudioPlayer::isStopped()
{
    if(player->playbackState()==QMediaPlayer::StoppedState) return true;
    return false;
}

qreal AudioPlayer::getVolume()
{
    return audioOutput->volume();
}
void AudioPlayer::setVolume(float volume)
{
    audioOutput->setVolume( volume );
}

void AudioPlayer::audioDecoder(QString filename)
{
    decoder->setSource(QUrl::fromLocalFile(filename));
}
