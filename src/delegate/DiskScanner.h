#pragma once
#include <QFileSystemWatcher>
#include <QMap>
#include <QObject>

// scan&watch media in specific path, and update ImageList

class DiskScanner : public QObject {
    Q_OBJECT

public:
    DiskScanner(QObject* parent = nullptr);

    // search path management
    // supposed to call scan() after:
    // - construct
    // - modify path through addPath(s)
    // remove path will also remove scanned image from model

    // recursively init path and subfolder
    void addPath(const QString& path);
    void addPaths(const QStringList& paths);
    // recursively deinit path and subfolder
    void removePath(const QString& path);
    void removePaths(const QStringList& path);
    QStringList path();

    // manually triggered scan, will trigger signal fullScan if(fullScan)
    void scan(bool fullScan = false);

signals:

    // supposed to replace all data in model
    void fullScan(const QStringList& filePath);
    void fileCreated(const QStringList& filePath);
    void fileDeleted(const QStringList& filePath);
    void fileModified(const QStringList& filePath);

private:
    bool initScanComplete = true;

    QStringList searchPath;
    QFileSystemWatcher diskWatcher;

    struct DiffResult {
        QStringList added;
        QStringList removed;
    };
    // compare and signal
    static DiffResult diff(const QStringList& oldv, const QStringList& newv);

    // scan specific path, path must be a subfolder of searchPath
    QMap<QString, QStringList> cache;
    // Cache for tracking file modification times
    QMap<QString, QMap<QString, QDateTime>> modCache;
    void scanPath(const QString& path, bool fullScan = false);

    // work with scan() and scanPath(), as scan cache
    QStringList pendingCreated, pendingDeleted, pendingModified;
    void submitChange(bool fullScan = false);

    // QDir name filter
    static const inline QStringList mediaFileFilter = {
        // image, ref: https://developer.mozilla.org/en-US/docs/Web/Media/Formats/Image_types
        "*.apng",
        "*.png",
        "*.avif",
        "*.gif",
        "*.jpg",
        "*.jpeg",
        "*.jfif",
        "*.pjpeg",
        "*.pjp",
        "*.png",
        "*.svg",
        "*.webp",
        "*.bmp",
        "*.ico",
        "*.cur",
        "*.tif",
        "*.tiff",

        // video, ref: https://en.wikipedia.org/wiki/Video_file_format
        // "*.webm",
        // "*.mkv",
        // "*.flv",
        // "*.flv",
        // "*.vob",
        // "*.ogv",
        // "*.ogg",
        // "*.drc",
        // "*.gif",
        // "*.gifv",
        // "*.mng",
        // "*.avi",
        // "*.MTS",
        // "*.M2TS",
        // "*.TS",
        // "*.mov",
        // "*.qt",
        // "*.wmv",
        // "*.yuv",
        // "*.rm",
        // "*.rmvb",
        // "*.viv",
        // "*.asf",
        // "*.amv",
        // "*.mp4",
        // "*.m4p",
        // "*.m4v",
        // "*.mpg",
        // "*.mp2",
        // "*.mpeg",
        // "*.mpe",
        // "*.mpv",
        // "*.mpg",
        // "*.mpeg",
        // "*.m2v",
        // "*.m4v",
        // "*.svi",
        // "*.3gp",
        // "*.3g2",
        // "*.mxf",
        // "*.roq",
        // "*.nsv",
        // "*.flv",
        // "*.f4v",
        // "*.f4p",
        // "*.f4a",
        // "*.f4b",
    };
};
