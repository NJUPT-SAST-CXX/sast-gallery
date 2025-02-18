#include "ImageViewer.h"
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>

ImageViewer::ImageViewer(QWidget* parent)
    : QGraphicsView(parent)
    , scene(new QGraphicsScene(this))
    , pixmapItem(new QGraphicsPixmapItem())
    , dragging(false)
    , cntScale(100)
    , minScale(1)
    , maxScale(800) {
    setScene(scene);
    scene->addItem(pixmapItem);
    setRenderHint(QPainter::Antialiasing);
    setRenderHint(QPainter::SmoothPixmapTransform);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setDragMode(QGraphicsView::ScrollHandDrag);
    setBackgroundBrush(Qt::transparent);
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    setStyleSheet("background-color: transparent;");
    setFrameShape(QFrame::NoFrame);
}

ImageViewer::ImageViewer(const QPixmap& pixmap, QWidget* parent)
    : ImageViewer(parent) {
    setContent(pixmap);
}

void ImageViewer::setContent(const QPixmap& pixmap, bool fadeAnimation) {
    if (pixmapItem->pixmap().isNull() || !fadeAnimation) {
        pixmapItem->setPixmap(pixmap);
        adjustImageToFit();
        return;
    }

    QGraphicsOpacityEffect* opacityEffect = new QGraphicsOpacityEffect(this);
    pixmapItem->setGraphicsEffect(opacityEffect);

    QPropertyAnimation* animation = new QPropertyAnimation(opacityEffect, "opacity");
    animation->setDuration(250);
    animation->setStartValue(0.0);
    animation->setEndValue(1.0);

    pixmapItem->setPixmap(pixmap);
    adjustImageToFit();
    animation->start(QPropertyAnimation::DeleteWhenStopped);
    cntScale = 100;
}

void ImageViewer::setContent(const QImage& image, bool fadeAnimation) {
    setContent(QPixmap::fromImage(image), fadeAnimation);
}

int ImageViewer::getScale() const {
    return cntScale;
}

int ImageViewer::getMinScale() const {
    return minScale;
}

int ImageViewer::getMaxScale() const {
    return maxScale;
}

void ImageViewer::setMinScale(int scale) {
    if (scale < 1) {
        minScale = 1;
    } else if (scale > maxScale) {
        minScale = maxScale;
    }
}

void ImageViewer::setMaxScale(int scale) {
    if (scale < minScale) {
        scale = minScale;
    }
    maxScale = scale;
}

void ImageViewer::scaleTo(int scale) {
    if (scale < minScale) {
        scale = minScale;
    } else if (scale > maxScale) {
        scale = maxScale;
    }
    const double scaleFactor = static_cast<double>(scale) / cntScale;
    this->scale(scaleFactor, scaleFactor);
    cntScale = scale;
}

void ImageViewer::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        lastMousePos = event->pos();
        dragging = true;
    }
    QGraphicsView::mousePressEvent(event);
}

void ImageViewer::mouseMoveEvent(QMouseEvent* event) {
    if (dragging) {
        QPointF delta = mapToScene(event->pos()) - mapToScene(lastMousePos);
        lastMousePos = event->pos();
        setTransformationAnchor(QGraphicsView::NoAnchor);
        translate(delta.x(), delta.y());
        setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    }
    QGraphicsView::mouseMoveEvent(event);
}

void ImageViewer::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        dragging = false;
    }
    QGraphicsView::mouseReleaseEvent(event);
}

void ImageViewer::resizeEvent(QResizeEvent* event) {
    QGraphicsView::resizeEvent(event);
    auto viewCenter = mapToScene(viewport()->rect().center());
    int cntScalePercent = cntScale;
    adjustImageToFit();
    centerOn(viewCenter);
    cntScale = 100;
    scaleTo(cntScalePercent);
}

void ImageViewer::wheelEvent(QWheelEvent* event) {
    emit wheelScrolled(event->angleDelta().y());
    event->accept();
}

void ImageViewer::adjustImageToFit() {
    if (!pixmapItem->pixmap().isNull()) {
        setSceneRect(pixmapItem->pixmap().rect());
        fitInView(pixmapItem, Qt::KeepAspectRatio);
    }
}
