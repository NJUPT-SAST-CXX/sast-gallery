#include "AbstractMediaViewer.h"

AbstractMediaViewer::AbstractMediaViewer(QWidget* parent)
    : QGraphicsView(parent)
    , scene(new QGraphicsScene(this))
    , dragging(false)
    , cntScale(100)
    , minScale(1)
    , maxScale(800) {
    setScene(scene);
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

int AbstractMediaViewer::getScale() const {
    return cntScale;
}

int AbstractMediaViewer::getMinScale() const {
    return minScale;
}

int AbstractMediaViewer::getMaxScale() const {
    return maxScale;
}

void AbstractMediaViewer::setMinScale(int scale) {
    if (scale < 1) {
        minScale = 1;
    } else if (scale > maxScale) {
        minScale = maxScale;
    }
}

void AbstractMediaViewer::setMaxScale(int scale) {
    if (scale < minScale) {
        scale = minScale;
    }
    maxScale = scale;
}

void AbstractMediaViewer::scaleTo(int scale) {
    if (scale < minScale) {
        scale = minScale;
    } else if (scale > maxScale) {
        scale = maxScale;
    }
    const double scaleFactor = static_cast<double>(scale) / cntScale;
    this->scale(scaleFactor, scaleFactor);
    cntScale = scale;
}

void AbstractMediaViewer::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        lastMousePos = event->pos();
        dragging = true;
    }
    QGraphicsView::mousePressEvent(event);
}

void AbstractMediaViewer::mouseMoveEvent(QMouseEvent* event) {
    if (dragging) {
        QPointF delta = mapToScene(event->pos()) - mapToScene(lastMousePos);
        lastMousePos = event->pos();
        setTransformationAnchor(QGraphicsView::NoAnchor);
        translate(delta.x(), delta.y());
        setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    }
    QGraphicsView::mouseMoveEvent(event);
}

void AbstractMediaViewer::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        dragging = false;
    }
    QGraphicsView::mouseReleaseEvent(event);
}

void AbstractMediaViewer::resizeEvent(QResizeEvent* event) {
    QGraphicsView::resizeEvent(event);
    auto viewCenter = mapToScene(viewport()->rect().center());
    int cntScalePercent = cntScale;
    adjustContentToFit();
    centerOn(viewCenter);
    cntScale = 100;
    scaleTo(cntScalePercent);
}

void AbstractMediaViewer::wheelEvent(QWheelEvent* event) {
    emit wheelScrolled(event->angleDelta().y());
    event->accept();
}