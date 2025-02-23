#pragma once

#include "AbstractMediaViewer.h"

class ImageViewer : public AbstractMediaViewer {
    Q_OBJECT

public:
    explicit ImageViewer(QWidget* parent = nullptr);
    explicit ImageViewer(const QPixmap& pixmap, QWidget* parent = nullptr);
};
