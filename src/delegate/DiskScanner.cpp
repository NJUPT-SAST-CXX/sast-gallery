#include "DiskScanner.h"
#include <QDir>
#include <QDirIterator>
#include <QStandardPaths>

DiskScanner::DiskScanner(QObject* parent)
    : QObject(parent) {
    connect(&diskWatcher, &QFileSystemWatcher::directoryChanged, [this](const QString& path) {
        scanPath(path); //扫描
        submitChange(); //提交结果
    });

    addPaths(QStandardPaths::standardLocations(QStandardPaths::PicturesLocation));
    addPaths(QStandardPaths::standardLocations(QStandardPaths::MoviesLocation));
} //添加扫描路径

void DiskScanner::addPath(const QString& path) { //路径管理
    if (searchPath.contains(path)) {
        return;
    } //避免重复添加

    searchPath.append(path); //如果不在则添加到末尾

    QStringList pendingPath;
    pendingPath.append(path); //存储待处理的路径
                              //遍历目录       //排除父目录和当前目录
    QDirIterator it(path, QDir::Dirs | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
    while (it.hasNext()) {             //检查是否有下一个目录
        pendingPath.append(it.next()); //返回下一个目录路径并添加到pP
    }

    for (auto& path : pendingPath) {
        if (diskWatcher.directories().contains(path)) { //检查是否已经在监视列表
            continue;
        }
        diskWatcher.addPath(path); //如果不在则添加到监视器

        qInfo() << "DiskScanner: path added to disk watcher: " << path; //输出日志
    }
}

void DiskScanner::addPaths(const QStringList& paths) {
    for (const auto& path : paths) {
        addPath(path);
    }
} //遍历路径列表，添加路径

void DiskScanner::removePath(const QString& path) {
    if (!searchPath.removeOne(path)) {
        return;
    } //如果移除失败则返回

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
        diskWatcher.removePath(path); //如果还在就移除
        scanPath(path);

        qInfo() << "DiskScanner: path removed from disk watcher: " << path;
    }
    submitChange();
}

void DiskScanner::removePaths(const QStringList& paths) {
    for (const auto& path : paths) {
        removePath(path);
    }
} //遍历路径列表，移除路径

QStringList DiskScanner::path() {
    return searchPath;
} //返回当前监控路径列表

void DiskScanner::scan(bool fullScan) {
    static bool isInitScan = true;     //若为首次扫描
    fullScan = fullScan || isInitScan; //则进行全量扫描
    isInitScan = false;
    for (auto& path : diskWatcher.directories()) {
        scanPath(path, fullScan); //扫描所有路径
    }
    submitChange(fullScan); //提交扫描结果
}

void DiskScanner::scanPath(const QString& path, bool fullScan) {
    if (!diskWatcher.directories().contains(path)) {
        // run remove
        pendingDeleted += cache.take(path);
        return;
    } //如果已经删除了，就把缓存也删了然后存到pD里

    qDebug() << "DiskScanner: scanning " << path;
    QStringList oldCache = fullScan ? QStringList{} : cache.value(path);
    QStringList newCache;                                            //存储当前扫描到的文件列表
    auto&& entryInfoList = QDir(path).entryInfoList(mediaFileFilter, //过滤文件
                                                    QDir::Files | QDir::NoDotAndDotDot);
    for (auto& entry : entryInfoList) {
        QString filePath = entry.absoluteFilePath(); //保存该文件的完整路径
        newCache += entry.absoluteFilePath();        //将路径添加到newcache中

        QDateTime currentModificationTime = entry.lastModified(); //获取文件的最后修改时间
        if (fileModificationTimes.contains(filePath)) {           //检查是否记录修改时间
            QDateTime oldModificationTime = fileModificationTimes[filePath]; //获取之前的修改时间
            if (currentModificationTime != oldModificationTime) {
                pendingModified.append(filePath); //存储被修改的文件的文件路径
            }
        }
        fileModificationTimes[filePath] = currentModificationTime; //更新修改时间
    }
    cache.insert(path, newCache); //关联path和文件列表

    auto&& [added, removed] = diff(oldCache, newCache); //比较差异，进行分类
    pendingCreated += added;
    pendingDeleted += removed;
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
        emit fileModified(pendingModified);
        pendingModified.clear();
    }
} //扫描处理

DiskScanner::DiffResult DiskScanner::diff(const QStringList& oldv, const QStringList& newv) {
    QSet olds(oldv.begin(), oldv.end());
    QSet news(newv.begin(), newv.end());
    DiffResult res;
    res.added = [=]() {
        auto res = news - olds;
        return QStringList(res.begin(), res.end());
    }(); //计算新增的
    res.removed = [=]() {
        auto res = olds - news;
        return QStringList(res.begin(), res.end());
    }();
    return res;
} //差异计算
