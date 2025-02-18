#include "DiskScanner.h"
#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QDirIterator>
#include <QFileInfo>
#include <QStandardPaths>

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
    auto&& entryInfoList = QDir(path).entryInfoList(mediaFileFilter,
                                                    QDir::Files | QDir::NoDotAndDotDot);
    for (auto&& entry : entryInfoList) {
        QString filePath = entry.absoluteFilePath();
        newCache += filePath;
    }
    cache.insert(path, newCache);

    auto&& [added, removed, modified] = diff(oldCache, newCache);
    pendingCreated += added;
    pendingDeleted += removed;
    pendingModified += modified;
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
        qDebug() << "DiskScanner: emitting fileCreated signal for " << pendingCreated.size()
                 << " files";
        emit fileCreated(pendingCreated);
        pendingCreated.clear();
    }
    if (pendingDeleted.size() != 0) {
        qDebug() << "DiskScanner: emitting fileDeleted signal for " << pendingDeleted.size()
                 << " files";
        emit fileDeleted(pendingDeleted);
        pendingDeleted.clear();
    }
    if (pendingModified.size() != 0) {
        qDebug() << "DiskScanner: emitting fileModified signal for " << pendingModified.size()
                 << " files";
        emit fileModified(pendingModified);
        pendingModified.clear();
    }
}

DiskScanner::DiffResult DiskScanner::diff(const QStringList& oldv, const QStringList& newv) {
    QSet<QString> olds(oldv.begin(), oldv.end());
    QSet<QString> news(newv.begin(), newv.end());

    DiffResult res;
    res.added = [=]() {
        auto res = news - olds;
        return QStringList(res.begin(), res.end());
    }();

    res.removed = [=]() {
        auto res = olds - news;
        return QStringList(res.begin(), res.end());
    }();

    res.modified = [=]() {
        QStringList res;
        for (const auto& filePath : newv) {
            if (olds.contains(filePath)) {
                QDateTime newModTime = QFileInfo(filePath).lastModified();
                QDateTime oldModTime = oldModTimes.value(filePath);

                if (newModTime.isValid() && oldModTime.isValid() && newModTime != oldModTime) {
                    qDebug() << "Modified file:" << filePath;
                    res += filePath;
                }
            }
        }
        return res;
    }();

    // Update the modification times for the next comparison
    for (const auto& filePath : res.added + res.modified) {
        QDateTime newModTime = QFileInfo(filePath).lastModified();
        if (newModTime.isValid()) {
            oldModTimes[filePath] = newModTime;
        }
    }

    for (const auto& filePath : res.removed) {
        oldModTimes.remove(filePath);
    }

    return res;
}
