#include "VideoViewer.h"
#include <QHBoxLayout>
#include <QPainter>
#include <QStyle>
#include <QUrl>
#include <QVBoxLayout>

VideoViewer::VideoViewer(QWidget* parent)
    : AbstractMediaViewer(parent)
    , m_mediaPlayer(nullptr)
    , m_videoWidget(nullptr)
    , m_playButton(nullptr)
    , m_positionSlider(nullptr)
    , m_timeLabel(nullptr)
    , m_errorLabel(nullptr) {
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

VideoViewer::VideoViewer(const QPixmap& pixmap, QWidget* parent)
    : AbstractMediaViewer(pixmap, parent)
    , m_mediaPlayer(nullptr)
    , m_videoWidget(nullptr)
    , m_playButton(nullptr)
    , m_positionSlider(nullptr)
    , m_timeLabel(nullptr)
    , m_errorLabel(nullptr) {
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

VideoViewer::VideoViewer(const QString& filePath, QWidget* parent)
    : AbstractMediaViewer(parent)
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

void VideoViewer::resizeEvent(QResizeEvent* event) {
    AbstractMediaViewer::resizeEvent(event);
    if (m_videoWidget) {
        m_videoWidget->resize(size());
    }
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
    mainLayout->setSpacing(0);

    // Video widget
    mainLayout->addWidget(m_videoWidget, 1);

    // Controls container widget - 使用容器来设置背景和固定高度
    auto* controlsContainer = new QWidget(this);
    controlsContainer->setFixedHeight(40);
    controlsContainer->setStyleSheet("background-color: rgba(0, 0, 0, 0.6);");

    auto* controlLayout = new QHBoxLayout(controlsContainer);
    controlLayout->setContentsMargins(8, 0, 8, 0);
    controlLayout->setSpacing(4);

    // Play button
    m_playButton = new QPushButton(this);
    m_playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    m_playButton->setFixedSize(28, 28);
    m_playButton->setStyleSheet("QPushButton { border: none; background: transparent; } "
                                "QPushButton:hover { background: rgba(255, 255, 255, 0.1); }");

    // Position slider - 创建自定义的QSlider来处理点击事件
    class ClickableSlider : public QSlider {
    protected:
        void mousePressEvent(QMouseEvent* event) override {
            if (event->button() == Qt::LeftButton) {
                if (orientation() == Qt::Horizontal) {
                    double pos = event->pos().x() / (double) width();
                    setValue(pos * (maximum() - minimum()) + minimum());
                }
                event->accept();
            }
            QSlider::mousePressEvent(event);
        }
    };

    m_positionSlider = new ClickableSlider();
    m_positionSlider->setOrientation(Qt::Horizontal);
    m_positionSlider->setRange(0, 0);
    m_positionSlider->setFixedHeight(20);
    m_positionSlider->setStyleSheet("QSlider::groove:horizontal {"
                                    "    height: 4px;"
                                    "    background: #4a4a4a;"
                                    "    margin: 0 0;"
                                    "}"
                                    "QSlider::handle:horizontal {"
                                    "    background: white;"
                                    "    width: 10px;"
                                    "    margin: -3px 0;"
                                    "    border-radius: 5px;"
                                    "}"
                                    "QSlider::sub-page:horizontal {"
                                    "    background: #0078d4;"
                                    "}");

    // Time label
    m_timeLabel = new QLabel("00:00 / 00:00", this);
    m_timeLabel->setMinimumWidth(80);
    m_timeLabel->setStyleSheet("color: white; font-size: 11px;");
    m_timeLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    controlLayout->addWidget(m_playButton);
    controlLayout->addWidget(m_positionSlider, 1);
    controlLayout->addWidget(m_timeLabel);

    mainLayout->addWidget(controlsContainer);

    // Error label
    m_errorLabel = new QLabel(this);
    m_errorLabel->setStyleSheet("color: red; background: rgba(0, 0, 0, 0.6); padding: 4px;");
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

    // 连接滑块的值变化信号
    connect(m_positionSlider, &QSlider::valueChanged, this, [this](int value) {
        m_mediaPlayer->setPosition(value);
    });
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