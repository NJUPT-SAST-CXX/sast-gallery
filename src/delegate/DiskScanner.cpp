#include "DiskScanner.h"
#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QDirIterator>
#include <QFileInfo>
#include <QStandardPaths>

// TODO: add filter
DiskScanner::DiskScanner(QObject* parent)
    : QObject(parent) {
    connect(&diskWatcher, &QFileSystemWatcher::directoryChanged, [this](const QString& path) {
        qDebug() << "Directory changed:" << path;
        scanPath(path);
        submitChange();
    });

    addPaths(QStandardPaths::standardLocations(QStandardPaths::PicturesLocation));
    addPaths(QStandardPaths::standardLocations(QStandardPaths::MoviesLocation));
}

void DiskScanner::addPath(const QString& path) {
    if (searchPath.contains(path)) {
        return;
    }

    searchPath.append(path);

    QStringList pendingPath;
    pendingPath.append(path);

    QDirIterator it(path, QDir::Dirs | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        pendingPath.append(it.next());
    }

    for (auto& path : pendingPath) {
        if (diskWatcher.directories().contains(path)) {
            continue;
        }
        diskWatcher.addPath(path);

        qInfo() << "DiskScanner: path added to disk watcher: " << path;
    }
}

void DiskScanner::addPaths(const QStringList& paths) {
    for (const auto& path : paths) {
        addPath(path);
    }
}

void DiskScanner::removePath(const QString& path) {
    if (!searchPath.removeOne(path)) {
        return;
    }

    QStringList pendingPath;
    pendingPath.append(path);

    QDirIterator it(path, QDir::Dirs | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        pendingPath.append(it.next());
    }

    for (auto& path : pendingPath) {
        if (!diskWatcher.directories().contains(path)) {
            continue;
        }
        diskWatcher.removePath(path);
        scanPath(path);

        qInfo() << "DiskScanner: path removed from disk watcher: " << path;
    }
    submitChange();
}

void DiskScanner::removePaths(const QStringList& paths) {
    for (const auto& path : paths) {
        removePath(path);
    }
}

QStringList DiskScanner::path() {
    return searchPath;
}

void DiskScanner::scan(bool fullScan) {
    static bool isInitScan = true;
    fullScan = fullScan || isInitScan;
    isInitScan = false;
    for (auto& path : diskWatcher.directories()) {
        scanPath(path, fullScan);
    }
    submitChange(fullScan);
}

void DiskScanner::scanPath(const QString& path, bool fullScan) {
    if (!diskWatcher.directories().contains(path)) {
        // run remove
        pendingDeleted += cache.take(path);
        return;
    }

    qDebug() << "DiskScanner: scanning " << path;
    QStringList oldCache = fullScan ? QStringList{} : cache.value(path);
    QStringList newCache;
    QMap<QString, QDateTime> newModTimes;
    auto&& entryInfoList = QDir(path).entryInfoList(mediaFileFilter,
                                                    QDir::Files | QDir::NoDotAndDotDot);
    for (auto&& entry : entryInfoList) {
        QString filePath = entry.absoluteFilePath();
        //qDebug() << "Scanning file: " << filePath << ", last modified: " << entry.lastModified();
        newCache += filePath;
        newModTimes[filePath] = entry.lastModified(); // cache current mod time
    }
    cache.insert(path, newCache);

    auto&& [added, removed] = diff(oldCache, newCache);
    pendingCreated += added;
    pendingDeleted += removed;

    // Check for modified files
    for (const auto& filePath : newCache) {
        if (oldCache.contains(filePath)) {
            QDateTime oldModTime = oldModTimes.value(filePath);
            QDateTime newModTime = newModTimes[filePath];
            //qDebug() << "Checking for modified file: " << filePath;
            //qDebug() << "old mod time: " << oldModTime << ", new mod time: " << newModTime;
            if (oldModTime.isValid() && oldModTime != newModTime) {
                qDebug() << "Modified file: " << filePath;
                pendingModified += filePath;
            }
        }
    }

    // update mod times cache
    for (const auto& filePath : newCache) {
        oldModTimes[filePath] = newModTimes[filePath];
    }

    for (const auto& filePath : removed) {
        oldModTimes.remove(filePath); // remove mod time for removed files
    }
}

void DiskScanner::submitChange(bool fullScan) {
    if (fullScan) {
        emit DiskScanner::fullScan(pendingCreated);
        pendingCreated.clear();
        pendingDeleted.clear();
        pendingModified.clear();
        return;
    }
    if (pendingCreated.size() != 0) {
        emit fileCreated(pendingCreated);
        pendingCreated.clear();
    }
    if (pendingDeleted.size() != 0) {
        emit fileDeleted(pendingDeleted);
        pendingDeleted.clear();
    }
    if (pendingModified.size() != 0) {
        // qDebug() << "Emitting fileModified signal for: " << pendingModified;
        emit fileModified(pendingModified);
        pendingModified.clear();
    }
}

DiskScanner::DiffResult DiskScanner::diff(const QStringList& oldv, const QStringList& newv) {
    QSet olds(oldv.begin(), oldv.end());
    QSet news(newv.begin(), newv.end());
    DiffResult res;
    res.added = [=]() {
        auto res = news - olds;
        return QStringList(res.begin(), res.end());
    }();
    res.removed = [=]() {
        auto res = olds - news;
        return QStringList(res.begin(), res.end());
    }();
    return res;
}
