#pragma once

#include <QLabel>
#include <QMediaPlayer>
#include <QPushButton>
#include <QSlider>
#include <QVideoWidget>
#include <QWidget>

class VideoViewer : public QWidget {
    Q_OBJECT

public:
    explicit VideoViewer(const QString& filePath, QWidget* parent = nullptr);
    ~VideoViewer();

protected:
    void paintEvent(QPaintEvent* event) override;
    QSize sizeHint() const override;

private slots:
    void handleError(QMediaPlayer::Error error);
    void updateDuration(qint64 duration);
    void updatePosition(qint64 position);
    void updatePlaybackState(QMediaPlayer::PlaybackState state);

private:
    void initializePlayer();
    void initializeUI();
    void setupConnections();
    QString formatTime(qint64 milliseconds);

    QString m_filePath;
    QMediaPlayer* m_mediaPlayer;
    QVideoWidget* m_videoWidget;
    QPushButton* m_playButton;
    QSlider* m_positionSlider;
    QLabel* m_timeLabel;
    QLabel* m_errorLabel;
};
