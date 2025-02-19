#pragma once

#include <QDateTime>
#include <QFileInfo>
#include <QString>

enum class MediaType {
    Image,
    Video,
    Unknown
};

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
        static const QStringList videoExtensions = {
            "mp4", "avi", "mov", "wmv", "mkv", "webm", "flv", "m4v"
        };
        return videoExtensions.contains(extension);
    }

    static bool isImageExtension(const QString& extension) {
        static const QStringList imageExtensions = {
            "jpg", "jpeg", "png", "gif", "bmp", "webp", "tiff"
        };
        return imageExtensions.contains(extension);
    }
}; 