#pragma once

#include <ElaIconButton.h>
#include <QImage>
#include <QString>

struct Tools {
    Tools() = delete;
    static QString fileSizeString(QString filepath);
    static int imageBitDepth(const QImage& image);
};