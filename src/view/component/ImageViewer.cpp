#include "ImageViewer.h"

ImageViewer::ImageViewer(QWidget* parent)
    : AbstractMediaViewer(parent) {}

ImageViewer::ImageViewer(const QPixmap& pixmap, QWidget* parent)
    : AbstractMediaViewer(pixmap, parent) {}
