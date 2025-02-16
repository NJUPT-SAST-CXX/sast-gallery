#include "DiskScanner.h"
#include <QDir>
#include <QDirIterator>
#include <QStandardPaths>

DiskScanner::DiskScanner(QObject* parent)
    : QObject(parent) {
    connect(&diskWatcher, &QFileSystemWatcher::directoryChanged, [this](const QString& path) {
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
    
    QStringList modifiedFiles;
    
    for (auto& entry : entryInfoList) {
        QString filePath = entry.absoluteFilePath();
        newCache += filePath;
        
        // 检查文件是否被修改
        if (oldCache.contains(filePath) && isFileModified(filePath)) {
            modifiedFiles += filePath;
        }
    }
    
    cache.insert(path, newCache);

    auto&& [added, removed] = diff(oldCache, newCache);
    pendingCreated += added;
    pendingDeleted += removed;
    
    // 更新所有新文件的最后修改时间缓存
    updateModifiedTimeCache(newCache);
    
    // 发送文件修改信号
    if (!modifiedFiles.isEmpty()) {
        emit fileModified(modifiedFiles);
    }
}

bool DiskScanner::isFileModified(const QString& filePath) {
    QFileInfo fileInfo(filePath);
    if (!fileInfo.exists()) return false;
    
    QDateTime lastModified = fileInfo.lastModified();
    
    // 如果文件不在缓存中，视为未修改
    if (!lastModifiedTimeCache.contains(filePath)) {
        return false;
    }
    
    // 比较文件的最后修改时间
    return lastModified != lastModifiedTimeCache[filePath];
}

void DiskScanner::updateModifiedTimeCache(const QString& filePath) {
    QFileInfo fileInfo(filePath);
    if (fileInfo.exists()) {
        lastModifiedTimeCache[filePath] = fileInfo.lastModified();
    }
}

void DiskScanner::updateModifiedTimeCache(const QStringList& filePaths) {
    for (const auto& filePath : filePaths) {
        updateModifiedTimeCache(filePath);
    }
}

void DiskScanner::submitChange(bool fullScan) {
    if (fullScan) {
        emit DiskScanner::fullScan(pendingCreated);
        pendingCreated.clear();
        pendingDeleted.clear();
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
