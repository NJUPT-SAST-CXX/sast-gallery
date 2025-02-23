#pragma once

#include "AbstractMediaViewer.h"

class VideoViewer : public AbstractMediaViewer {
    Q_OBJECT

public:
    explicit VideoViewer(QWidget* parent = nullptr);
    explicit VideoViewer(const QPixmap& pixmap, QWidget* parent = nullptr);

private:

};
