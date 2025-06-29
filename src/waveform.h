#ifndef WAVEFORM_H
#define WAVEFORM_H

#include <QWidget>
#include <QMouseEvent>

class Waveform : public QWidget
{
    Q_OBJECT

    public:
        explicit Waveform(QWidget *parent = nullptr);

        void setPlaybackPosition(qint64 pos);
        void setTotalDuration(qint64 duration);
        void empty();

    public slots:
            void setWaveform(const QVector<float> &data);

    protected:
        void paintEvent(QPaintEvent *event) override;
        void mousePressEvent(QMouseEvent *event) override;
        void mouseMoveEvent(QMouseEvent *event) override;
        void mouseReleaseEvent(QMouseEvent *event) override;
        void leaveEvent(QEvent *) override;

    signals:
        void seekRequested(qint64 position);

    private:
        QVector<float> waveform;
        QVector<float> precomputed;

        qint64 playbackPosition = 0;
        qint64 totalDuration = 1;
        bool seeking = false;
        float previewRatio = -1.0f;
        float hoverRatio = -1.0f;
};

#endif // WAVEFORM_H
