#pragma once
#include <QFileSystemWatcher> //监控文件和目录变化
#include <QMap>               //存储扫描结果的缓存
#include <QObject>

// scan&watch media in specific path, and update ImageList

class DiskScanner : public QObject {
    Q_OBJECT

public:
    DiskScanner();

    // search path management
    // supposed to call scan() after:
    // - construct
    // - modify path through addPath(s)
    // remove path will also remove scanned image from model

    // recursively init path and subfolder
    void addPath(const QString& path);       //添加单个或多个搜索路径
    void addPaths(const QStringList& paths); //并递归初始化这些路径
    // recursively deinit path and subfolder
    void removePath(const QString& path); //移除路径
    void removePaths(const QStringList& path);
    QStringList path(); //返回当前搜索路径列表

    // manually triggered scan, will trigger signal fullScan if(fullScan)
    void scan(bool fullScan = false); //手动触发扫描操作

signals:

    // supposed to replace all data in model
    void fullScan(const QStringList& filePath);     //全量扫描
    void fileCreated(const QStringList& filePath);  //创建新文件
    void fileDeleted(const QStringList& filePath);  //有文件被删除
    void fileModified(const QStringList& filePath); //有文件被修改 // TODO: detect file modified

private:
    bool initScanComplete = true; //初始扫描是否完成

    QStringList searchPath;         //存储当前的搜索路径列表
    QFileSystemWatcher diskWatcher; //用于监控文件系统的变化

    struct DiffResult {
        QStringList added;
        QStringList removed;
    }; //存储差异结果
    // compare and signal
    static DiffResult diff(const QStringList& oldv, const QStringList& newv);
    //用于比较两个文件列表的差异
    // scan specific path, path must be a subfolder of searchPath
    QMap<QString, QStringList> cache; //缓存每个路径下的缓存结果
    QMap<QString, QDateTime> fileModificationTimes;
    void scanPath(const QString& path, bool fullScan = false); //扫描指定路径下的文件

    // work with scan() and scanPath(), as scan cache
    QStringList pendingCreated, pendingDeleted, pendingModified; //暂存袋提交的新增和删除的文件列表
    void submitChange(bool fullScan = false);                    //提交文件变化，触发相应信号

    // QDir name filter
    static const inline QStringList mediaFileFilter = {
        //过滤媒体文件
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
