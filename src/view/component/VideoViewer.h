#pragma once

#include "AbstractMediaViewer.h"
#include <QLabel>
#include <QMediaPlayer>
#include <QPushButton>
#include <QSlider>
#include <QVideoWidget>

class VideoViewer : public AbstractMediaViewer {
    Q_OBJECT

public:
    explicit VideoViewer(QWidget* parent = nullptr);
    explicit VideoViewer(const QPixmap& pixmap, QWidget* parent = nullptr);
    explicit VideoViewer(const QString& filePath, QWidget* parent = nullptr);
    ~VideoViewer();

protected:
    void resizeEvent(QResizeEvent* event) override;

private slots:
    void handleError(QMediaPlayer::Error error);
    void updateDuration(qint64 duration);
    void updatePosition(qint64 position);
    void updatePlaybackState(QMediaPlayer::PlaybackState state);

private:
    QString m_filePath;
    QMediaPlayer* m_mediaPlayer;
    QVideoWidget* m_videoWidget;
    QPushButton* m_playButton;
    QSlider* m_positionSlider;
    QLabel* m_timeLabel;
    QLabel* m_errorLabel;

    void initializePlayer();
    void initializeUI();
    void setupConnections();
    QString formatTime(qint64 milliseconds);
};
