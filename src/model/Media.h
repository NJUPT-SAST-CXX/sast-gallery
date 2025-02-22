#pragma once

#include "../delegate/DiskScanner.h"
#include <QCryptographicHash>
#include <QDateTime>
#include <QDir>
#include <QFileInfo>
#include <QString>

enum class MediaType { Image, Video, Unknown };

class Media {
public:
    MediaType type;
    QString path;
    QDateTime lastModified;
    bool isFavorite;

    Media(const QString& filePath, const QDateTime& modTime = QDateTime(), bool favorite = false)
        : path(filePath)
        , lastModified(modTime)
        , isFavorite(favorite) {
        type = determineType(filePath);
    }

    QString getThumbnailPath() const {
        if (type != MediaType::Video)
            return path;

        // Create thumbnails directory if it doesn't exist
        QDir appDir = QDir::current();
        QString thumbDir = appDir.absoluteFilePath("data/thumbnails");
        QDir().mkpath(thumbDir);

        // Generate unique filename based on video path and last modified time
        QString hashInput = path + lastModified.toString(Qt::ISODate);
        QString hash = QString(
            QCryptographicHash::hash(hashInput.toUtf8(), QCryptographicHash::Md5).toHex());

        return thumbDir + "/" + hash + ".jpg";
    }

    bool hasThumbnail() const {
        if (type != MediaType::Video)
            return false;
        return QFile::exists(getThumbnailPath());
    }

    static MediaType determineType(const QString& filePath) {
        QString extension = QFileInfo(filePath).suffix().toLower();
        if (isVideoExtension(extension)) {
            return MediaType::Video;
        }
        if (isImageExtension(extension)) {
            return MediaType::Image;
        }
        return MediaType::Unknown;
    }

    static bool isVideoExtension(const QString& extension) {
        for (const QString& pattern : DiskScanner::videoFileFilter) {
            if (pattern.mid(2) == extension) {
                return true;
            }
        }
        return false;
    }

    static bool isImageExtension(const QString& extension) {
        for (const QString& pattern : DiskScanner::imageFileFilter) {
            if (pattern.mid(2) == extension) {
                return true;
            }
        }
        return false;
    }
};