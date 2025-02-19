#pragma once

#include <QAbstractItemModel>
#include <QImage>
#include <QPersistentModelIndex>
#include <QVBoxLayout>
#include <QWheelEvent>

class MediaViewer;

class MediaViewerDelegate : public QObject {
    Q_OBJECT

public:
    explicit MediaViewerDelegate(QAbstractItemModel* model,  //关联的数据模型，用于获取媒体文件信息
                                 int index,                  //媒体项在模型中的索引
                                 MediaViewer* viewer,        //媒体查看器窗口，用于显示媒体文件
                                 QObject* parent = nullptr); //可选的父对象，管理生命周期

    [[nodiscard]] auto getFilePath() const { return filepath; }
    [[nodiscard]] auto getImage() const { return this->image; }
    void initConnections();
    void setFilePath(const QString& path);

signals:
    void scaledByWheel();
    void imageChanged(bool fadeAnimation = true);

public slots:
    void onModelRowsToBeRemoved(const QModelIndex& parent, int first, int last); //处理媒体项被删除
    void onImageChanged(bool fadeAnimation = true);
    void onWheelScrolled(int delta);
    bool copyImageToClipboard(); //将当前图像复制到剪切板
    void openImageFileDialog();  //打开文件对话框,用于选择要查看的图像文件
    void saveImageFileDialog();  //打开保存文件的对话框，用于保存当前图像
    void onFileInfoClicked();    //
    void adaptiveResize();       //自适应调整图像的大小以适应查看器窗口
    void deleteImage();          //删除当前图像
    void prevImage();            //切换到上一图像
    void nextImage();            //切换到下一图像
    void rotateImage();          //旋转当前图像
    void openInFileExplorer();

private:
    QAbstractItemModel* mediaListModel;
    QPersistentModelIndex mediaIndex;
    QImage image;
    QString filepath;
    MediaViewer* view;
    QVBoxLayout* layout;

    bool loadImage(const QString& path, bool fadeAnimation = true);
    bool loadImage(const QImage& image, bool fadeAnimation = true);
    void scaleTo(int percent);
    [[nodiscard]] int getScale() const;
};
