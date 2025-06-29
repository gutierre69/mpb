#include "waveform.h"
#include <QPainter>
#include <QDebug>

Waveform::Waveform(QWidget *parent)
    : QWidget(parent)
{
    setMinimumHeight(50);
    setMouseTracking(true);
    empty();
}

void Waveform::empty()
{
    for(int i=0; i<100; i++){
        waveform.push_back({0});
    }
    setWaveform(waveform);
    qDebug() << "empty";
}

void Waveform::setWaveform(const QVector<float> &data)
{
    waveform = data;

    const int barWidth = 3;
    const int gap = 1;
    const int totalBars = width() / (barWidth + gap);
    precomputed.clear();
    precomputed.reserve(totalBars);

    const int samplesPerBar = waveform.size() / totalBars;

    for (int i = 0; i < totalBars; ++i) {
        float maxVal = 0.0f;
        for (int j = 0; j < samplesPerBar; ++j) {
            int idx = i * samplesPerBar + j;
            if (idx < waveform.size())
                maxVal = std::max(maxVal, waveform[idx]);
        }

        precomputed.push_back(std::pow(maxVal, 0.6f));
    }

    update();
}

void Waveform::setPlaybackPosition(qint64 pos)
{
    if(playbackPosition==pos)
        return;

    playbackPosition = pos;
    update();
}

void Waveform::setTotalDuration(qint64 duration)
{
    totalDuration = duration;
}

void Waveform::mousePressEvent(QMouseEvent *event)
{
    if (totalDuration == 0) return;

    seeking = true;
    previewRatio = float(event->position().x()) / width();
    previewRatio = std::clamp(previewRatio, 0.0f, 1.0f);
    update();
}

void Waveform::mouseMoveEvent(QMouseEvent *event)
{
    if (totalDuration == 0) return;

    hoverRatio = float(event->position().x()) / width();
    hoverRatio = std::clamp(hoverRatio, 0.0f, 1.0f);
    update();
}

void Waveform::mouseReleaseEvent(QMouseEvent *event)
{
    if (!seeking || totalDuration == 0) return;

    seeking = false;
    previewRatio = float(event->position().x()) / width();
    previewRatio = std::clamp(previewRatio, 0.0f, 1.0f);

    qint64 pos = static_cast<qint64>(previewRatio * totalDuration);
    emit seekRequested(pos);

    previewRatio = -1.0f;
    update();
}

void Waveform::leaveEvent(QEvent *) {
    hoverRatio = -1.0f;
    update();
}

void Waveform::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    //p.fillRect(rect(), Qt::black);

    if (waveform.isEmpty()) return;

    const int w = width();
    const int h = height();
    const int barWidth = 3;
    const int gap = 1;
    const int totalBars = precomputed.size();

    int hoverBar = static_cast<int>(hoverRatio * totalBars);
    int progressBar = static_cast<int>((playbackPosition / float(totalDuration)) * totalBars);
    int previewBar = static_cast<int>(previewRatio * totalBars);

    for (int i = 0; i < totalBars; ++i) {
        float value = precomputed[i];
        int barHeight = std::max(static_cast<int>(value * h), 2);
        int x = i * (barWidth + gap);
        int y = (h - barHeight) / 2;

        QColor color;

        if (seeking && i <= previewBar)
            color = QColor("#00a1ff");
        else if (!seeking && hoverRatio >= 0 && i <= hoverBar) {
            color = (i <= progressBar) ? QColor("#72cbff") : QColor("#364d5b");
        } else if (i <= progressBar)
            color = QColor("#00a1ff");
        else
            color = QColor("#453f54");

        p.setBrush(color);
        p.setPen(Qt::NoPen);
        p.drawRect(x, y, barWidth, barHeight);
    }


}

