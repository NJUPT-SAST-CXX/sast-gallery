#include "VideoViewer.h"
#include <QHBoxLayout>
#include <QPainter>
#include <QStyle>
#include <QUrl>
#include <QVBoxLayout>

VideoViewer::VideoViewer(const QString& filePath, QWidget* parent)
    : QWidget(parent)
    , m_filePath(filePath)
    , m_mediaPlayer(nullptr)
    , m_videoWidget(nullptr)
    , m_playButton(nullptr)
    , m_positionSlider(nullptr)
    , m_timeLabel(nullptr)
    , m_errorLabel(nullptr) {
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    initializePlayer();
    initializeUI();
    setupConnections();
}

VideoViewer::~VideoViewer() {
    if (m_mediaPlayer) {
        m_mediaPlayer->stop();
        delete m_mediaPlayer;
    }
}

void VideoViewer::paintEvent(QPaintEvent* event) {
    // No need for custom painting as we use QVideoWidget
    QWidget::paintEvent(event);
}

QSize VideoViewer::sizeHint() const {
    return QSize(640, 480);
}

void VideoViewer::initializePlayer() {
    m_mediaPlayer = new QMediaPlayer(this);
    m_videoWidget = new QVideoWidget(this);
    m_mediaPlayer->setVideoOutput(m_videoWidget);
    m_mediaPlayer->setSource(QUrl::fromLocalFile(m_filePath));
}

void VideoViewer::initializeUI() {
    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    // Video widget
    mainLayout->addWidget(m_videoWidget);

    // Controls
    auto* controlLayout = new QHBoxLayout();
    controlLayout->setContentsMargins(8, 4, 8, 4);

    m_playButton = new QPushButton(this);
    m_playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    m_playButton->setFixedSize(32, 32);

    m_positionSlider = new QSlider(Qt::Horizontal, this);
    m_positionSlider->setRange(0, 0);

    m_timeLabel = new QLabel("00:00 / 00:00", this);
    m_timeLabel->setMinimumWidth(100);
    m_timeLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    controlLayout->addWidget(m_playButton);
    controlLayout->addWidget(m_positionSlider);
    controlLayout->addWidget(m_timeLabel);

    mainLayout->addLayout(controlLayout);

    // Error label
    m_errorLabel = new QLabel(this);
    m_errorLabel->setStyleSheet("color: red;");
    m_errorLabel->setAlignment(Qt::AlignCenter);
    m_errorLabel->hide();
    mainLayout->addWidget(m_errorLabel);
}

void VideoViewer::setupConnections() {
    connect(m_mediaPlayer, &QMediaPlayer::errorOccurred, this, &VideoViewer::handleError);
    connect(m_mediaPlayer, &QMediaPlayer::durationChanged, this, &VideoViewer::updateDuration);
    connect(m_mediaPlayer, &QMediaPlayer::positionChanged, this, &VideoViewer::updatePosition);
    connect(m_mediaPlayer,
            &QMediaPlayer::playbackStateChanged,
            this,
            &VideoViewer::updatePlaybackState);

    connect(m_playButton, &QPushButton::clicked, this, [this]() {
        if (m_mediaPlayer->playbackState() == QMediaPlayer::PlayingState) {
            m_mediaPlayer->pause();
        } else {
            m_mediaPlayer->play();
        }
    });

    connect(m_positionSlider, &QSlider::sliderMoved, m_mediaPlayer, &QMediaPlayer::setPosition);
}

void VideoViewer::handleError(QMediaPlayer::Error error) {
    QString errorStr = "Error: " + m_mediaPlayer->errorString();
    m_errorLabel->setText(errorStr);
    m_errorLabel->show();
}

void VideoViewer::updateDuration(qint64 duration) {
    m_positionSlider->setRange(0, duration);
    updatePosition(m_mediaPlayer->position());
}

void VideoViewer::updatePosition(qint64 position) {
    if (!m_positionSlider->isSliderDown()) {
        m_positionSlider->setValue(position);
    }

    QString timeStr = formatTime(position) + " / " + formatTime(m_mediaPlayer->duration());
    m_timeLabel->setText(timeStr);
}

void VideoViewer::updatePlaybackState(QMediaPlayer::PlaybackState state) {
    if (state == QMediaPlayer::PlayingState) {
        m_playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
    } else {
        m_playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    }
}

QString VideoViewer::formatTime(qint64 milliseconds) {
    qint64 seconds = milliseconds / 1000;
    qint64 minutes = seconds / 60;
    seconds = seconds % 60;
    return QString("%1:%2").arg(minutes, 2, 10, QChar('0')).arg(seconds, 2, 10, QChar('0'));
}
