#include "MediaPreviewer.h"
#include <QImageReader>
#include <QPainter>
#include <QPixmap>
#include <QPainterPath>
#include <QPropertyAnimation>
#include <QtConcurrentRun>
#include <model/MediaListModel.h>
#include "../MediaViewer.h"

MediaPreviewer::MediaPreviewer(QAbstractItemModel* model, int rowIndex, QWidget* parent)
    : QLabel(parent) {
    filepath = model->data(model->index(rowIndex, MediaListModel::Path)).value<QString>();
    lastModified = model->data(model->index(rowIndex, MediaListModel::LastModifiedTime))
                       .value<QDateTime>();
    isFav = model->data(model->index(rowIndex, MediaListModel::IsFavorite)).value<bool>();
    connect(&imageLoadWatcher,
            &QFutureWatcher<QPixmap*>::finished,
            this,
            &MediaPreviewer::loadImageComplete);
    setScaledContents(true);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    initMedia();
    // TODO: open the image in a MediaViewer window when double clicked
    _index=rowIndex;
    _model=model;
    connect(this,
            &MediaPreviewer::doubleClicked,
            this,
            &MediaPreviewer::on_doubleClicked);
}

MediaPreviewer::~MediaPreviewer() {}

void MediaPreviewer::paintEvent(QPaintEvent* event) {
    QLabel::paintEvent(event);
    if (requireReloadImage) {
        imageLoadWatcher.setFuture(QtConcurrent::run(&MediaPreviewer::loadImage, this));
        requireReloadImage = false;
    }
    //?
    QPainter painter(this);
    painter.drawPixmap(rect(),QPixmap(filepath));
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
    mediaSize = QImageReader(filepath).size();
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
//
void MediaPreviewer::on_doubleClicked(){
    MediaViewer *viewer=new MediaViewer(_model,_index);
    viewer->show();
}
