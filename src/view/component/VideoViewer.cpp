#include "VideoViewer.h"

VideoViewer::VideoViewer(QWidget* parent)
    : AbstractMediaViewer(parent) {}

VideoViewer::VideoViewer(const QPixmap& pixmap, QWidget* parent)
    : AbstractMediaViewer(pixmap, parent) {}