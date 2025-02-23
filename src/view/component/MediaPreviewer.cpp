#include "MediaPreviewer.h"
#include <QFileInfo>
#include <QImageReader>
#include <QMediaMetaData>
#include <QPainter>
#include <QPainterPath>
#include <QPropertyAnimation>
#include <QUrl>
#include <QVideoFrame>
#include <QtConcurrentRun>
#include <model/MediaListModel.h>
#include <view/MediaViewer.h>

MediaPreviewer::MediaPreviewer(QAbstractItemModel* model, int rowIndex, QWidget* parent)
    : QLabel(parent)
    , media(model->data(model->index(rowIndex, MediaListModel::Path)).value<QString>(),
            model->data(model->index(rowIndex, MediaListModel::LastModifiedTime)).value<QDateTime>(),
            model->data(model->index(rowIndex, MediaListModel::IsFavorite)).value<bool>()) {
    connect(&imageLoadWatcher,
            &QFutureWatcher<QPixmap*>::finished,
            this,
            &MediaPreviewer::loadImageComplete);
    connect(&videoLoadWatcher,
            &QFutureWatcher<QPixmap*>::finished,
            this,
            &MediaPreviewer::loadVideoComplete);
    setScaledContents(true);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    initMedia();

    connect(this, &MediaPreviewer::doubleClicked, this, [=, this]() {
        MediaViewer* viewer = new MediaViewer(model, rowIndex);
        viewer->setAttribute(Qt::WA_DeleteOnClose);
        viewer->show();
    });
}

MediaPreviewer::~MediaPreviewer() {}

void MediaPreviewer::paintEvent(QPaintEvent* event) {
    QLabel::paintEvent(event);
    if (requireReloadImage) {
        if (media.type == MediaType::Video) {
            videoLoadWatcher.setFuture(QtConcurrent::run(&MediaPreviewer::loadVideo, this));
        } else {
            imageLoadWatcher.setFuture(QtConcurrent::run(&MediaPreviewer::loadImage, this));
        }
        requireReloadImage = false;
    }
}

QSize MediaPreviewer::sizeHint() const {
    // return mediaSize.scaled(1000, 150, Qt::KeepAspectRatio);
    return mediaSize;
}

void MediaPreviewer::setPath(const QString& path) {
    media = Media(path, media.lastModified, media.isFavorite);
    initMedia();
}
void MediaPreviewer::setLastModifiedTime(const QDateTime& time) {
    media.lastModified = time;
    initMedia();
}

void MediaPreviewer::setIsFavorite(bool isFavorite) {
    media.isFavorite = isFavorite;
}

QString MediaPreviewer::path() {
    return media.path;
}

QDateTime MediaPreviewer::lastModifiedTime() {
    return media.lastModified;
}

bool MediaPreviewer::isFavorite() {
    return media.isFavorite;
}

void MediaPreviewer::initMedia() {
    if (media.type == MediaType::Video) {
        // Try to get video resolution from metadata first
        QMediaPlayer* player = new QMediaPlayer;
        QVideoWidget* videoWidget = new QVideoWidget;

        player->setVideoOutput(videoWidget);
        player->setSource(QUrl::fromLocalFile(media.path));

        connect(player, &QMediaPlayer::mediaStatusChanged, [=](QMediaPlayer::MediaStatus status) {
            if (status == QMediaPlayer::LoadedMedia) {
                mediaSize = player->metaData().value(QMediaMetaData::Resolution).toSize();
                player->deleteLater();
                videoWidget->deleteLater();
            }
        });
        qDebug() << "Video size:" << mediaSize;
    } else {
        mediaSize = QImageReader(media.path).size();
        qDebug() << "Image size:" << mediaSize;
    }
    requireReloadImage = true;
}

QPixmap MediaPreviewer::roundedPixmap(const QPixmap& original, double radius) {
    QPixmap target = QPixmap(original.size());
    target.fill(Qt::transparent);
    QPainter painter(&target);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    QPainterPath path = QPainterPath();
    path.addRoundedRect(target.rect(), radius, radius);
    painter.setClipPath(path);
    painter.drawPixmap(0, 0, original);
    return target;
}

void MediaPreviewer::loadImageComplete() {
    originalPixmap = imageLoadWatcher.result();
    setPixmap(originalPixmap);
}

QPixmap MediaPreviewer::loadImage() {
    QImageReader reader(media.path);
    reader.setScaledSize(QSize{0, 180});
    return roundedPixmap(QPixmap::fromImage(reader.read()), 4);
}

void MediaPreviewer::loadVideoComplete() {
    originalPixmap = videoLoadWatcher.result();
    setPixmap(originalPixmap);
}

QPixmap MediaPreviewer::loadVideo() {
    // Check if we already have a cached thumbnail
    if (media.hasThumbnail()) {
        QImageReader reader(media.getThumbnailPath());
        reader.setScaledSize(QSize{0, 180});
        return roundedPixmap(QPixmap::fromImage(reader.read()), 4);
    }

    // If no cached thumbnail, generate one
    QMediaPlayer player;
    QVideoSink videoSink;
    QVideoFrame frame;
    bool frameReceived = false;

    // Connect to receive video frames
    connect(&videoSink, &QVideoSink::videoFrameChanged, [&](const QVideoFrame& newFrame) {
        if (!frameReceived) { // Only take the first frame
            frame = newFrame;
            frameReceived = true;
            player.stop(); // Stop after getting first frame
        }
    });

    player.setVideoSink(&videoSink);
    player.setSource(QUrl::fromLocalFile(media.path));
    player.play();

    // Wait a bit for the frame
    for (int i = 0; i < 10 && !frameReceived; i++) {
        QThread::msleep(50);
        QCoreApplication::processEvents();
    }

    QImage thumbnail = frame.toImage().scaled(mediaSize,
                                              Qt::KeepAspectRatio,
                                              Qt::SmoothTransformation);

    thumbnail.save(media.getThumbnailPath());

    return roundedPixmap(QPixmap::fromImage(thumbnail), 4);
}

void MediaPreviewer::enterEvent(QEnterEvent* event) {
    QLabel::enterEvent(event);
    setCursor(Qt::PointingHandCursor);
}

void MediaPreviewer::leaveEvent(QEvent* event) {
    QLabel::leaveEvent(event);
    setCursor(Qt::ArrowCursor);
}

void MediaPreviewer::mousePressEvent(QMouseEvent* event) {
    QLabel::mousePressEvent(event);
    scaleAnimation(1.0, 0.95);
}

void MediaPreviewer::mouseReleaseEvent(QMouseEvent* event) {
    QLabel::mouseReleaseEvent(event);
    scaleAnimation(0.95, 1.0);
}

void MediaPreviewer::mouseDoubleClickEvent(QMouseEvent* event) {
    QLabel::mouseDoubleClickEvent(event);
    emit doubleClicked();
}

QPixmap MediaPreviewer::scalePixmapContent(qreal scaleFactor) {
    QSize originalSize = originalPixmap.size();
    QSize scaledSize = originalSize * scaleFactor;
    QPixmap target(originalPixmap.size());
    target.fill(Qt::transparent);

    QPainter painter(&target);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

    painter.drawPixmap((originalSize.width() - scaledSize.width()) / 2,
                       (originalSize.height() - scaledSize.height()) / 2,
                       originalPixmap.scaled(scaledSize,
                                             Qt::KeepAspectRatio,
                                             Qt::SmoothTransformation));
    return target;
}

void MediaPreviewer::scaleAnimation(qreal startScale, qreal endScale, int duration) {
    auto* animation = new QPropertyAnimation(this, "scaleFactor");
    animation->setDuration(duration);
    animation->setStartValue(startScale);
    animation->setEndValue(endScale);
    connect(animation, &QPropertyAnimation::valueChanged, this, [=, this]() {
        if (!originalPixmap.isNull()) {
            setPixmap(scalePixmapContent(animation->currentValue().toReal()));
        }
    });
    animation->start(QAbstractAnimation::DeleteWhenStopped);
}