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
    , isVideo(false) {
    filepath = model->data(model->index(rowIndex, MediaListModel::Path)).value<QString>();
    lastModified = model->data(model->index(rowIndex, MediaListModel::LastModifiedTime))
                       .value<QDateTime>();
    isFav = model->data(model->index(rowIndex, MediaListModel::IsFavorite)).value<bool>();
    connect(&imageLoadWatcher,
            &QFutureWatcher<QPixmap>::finished,
            this,
            &MediaPreviewer::loadImageComplete);
    connect(&videoLoadWatcher,
            &QFutureWatcher<QPixmap>::finished,
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
        if (isVideo) {
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
    filepath = path;
    initMedia();
}
void MediaPreviewer::setLastModifiedTime(const QDateTime& time) {
    lastModified = time;
    initMedia();
}

void MediaPreviewer::setIsFavorite(bool isFavorite) {
    isFav = isFavorite;
}

QString MediaPreviewer::path() {
    return filepath;
}

QDateTime MediaPreviewer::lastModifiedTime() {
    return lastModified;
}

bool MediaPreviewer::isFavorite() {
    return isFav;
}

void MediaPreviewer::initMedia() {
    isVideo = isVideoFile(filepath);
    if (isVideo) {
        // For video files, set a default size initially
        QMediaPlayer* player = new QMediaPlayer;
        QVideoWidget* videoWidget = new QVideoWidget;

        player->setVideoOutput(videoWidget);
        player->setSource(QUrl::fromLocalFile(filepath));

        connect(player, &QMediaPlayer::mediaStatusChanged, [=](QMediaPlayer::MediaStatus status) {
            if (status == QMediaPlayer::LoadedMedia) {
                mediaSize = player->metaData().value(QMediaMetaData::Resolution).toSize();
                qDebug() << "Media size:" << mediaSize;
            }
        });

        // player->play();

        // // Create a default video thumbnail with play icon
        // QPixmap defaultThumb(mediaSize);
        // defaultThumb.fill(Qt::black);
        // QPainter painter(&defaultThumb);

        // // Draw play icon
        // QPolygon playIcon;
        // int iconSize = 40;
        // playIcon << QPoint(mediaSize.width() / 2 - iconSize / 2,
        //                    mediaSize.height() / 2 - iconSize / 2)
        //          << QPoint(mediaSize.width() / 2 - iconSize / 2,
        //                    mediaSize.height() / 2 + iconSize / 2)
        //          << QPoint(mediaSize.width() / 2 + iconSize / 2, mediaSize.height() / 2);

        // painter.setPen(Qt::NoPen);
        // painter.setBrush(Qt::white);
        // painter.drawPolygon(playIcon);

        // originalPixmap = roundedPixmap(defaultThumb, 4);
        // setPixmap(originalPixmap);
    } else {
        mediaSize = QImageReader(filepath).size();
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
    QImageReader reader(filepath);
    reader.setScaledSize(QSize{0, 180});
    return roundedPixmap(QPixmap::fromImage(reader.read()), 4);
}

void MediaPreviewer::loadVideoComplete() {
    originalPixmap = videoLoadWatcher.result();
    setPixmap(originalPixmap);
}

QPixmap MediaPreviewer::loadVideo() {
    QMediaPlayer player;
    QVideoSink videoSink;
    player.setVideoSink(&videoSink);
    player.setSource(QUrl::fromLocalFile(filepath));

    // Create default thumbnail
    QPixmap defaultThumb(QSize(320, 180));
    defaultThumb.fill(Qt::black);
    QPainter painter(&defaultThumb);

    // Draw play icon
    QPolygon playIcon;
    int iconSize = 40;
    playIcon << QPoint(160 - iconSize / 2, 90 - iconSize / 2)
             << QPoint(160 - iconSize / 2, 90 + iconSize / 2) << QPoint(160 + iconSize / 2, 90);

    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::white);
    painter.drawPolygon(playIcon);

    return roundedPixmap(defaultThumb, 4);
}

// TODO
bool MediaPreviewer::isVideoFile(const QString& path) const {
    QString extension = QFileInfo(path).suffix().toLower();
    return extension == "mp4" || extension == "avi" || extension == "mov" || extension == "wmv"
           || extension == "mkv" || extension == "webm" || extension == "flv" || extension == "m4v";
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