#include "ImageViewer.h"
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>

ImageViewer::ImageViewer(QWidget* parent)
    : QGraphicsView(parent)
    , scene(new QGraphicsScene(this))
    , pixmapItem(new QGraphicsPixmapItem())
    , dragging(false) {
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
}

void ImageViewer::setContent(const QImage& image, bool fadeAnimation) {
    setContent(QPixmap::fromImage(image), fadeAnimation);
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
    adjustImageToFit();
    emit resized();
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