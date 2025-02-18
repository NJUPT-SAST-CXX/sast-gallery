#pragma once

#include <QAbstractItemModel>
#include <QImage>
#include <QObject>
#include <QPersistentModelIndex>
#include <QVBoxLayout>
#include <QWheelEvent>

class MediaViewer;

class MediaViewerDelegate : public QObject {
    Q_OBJECT

public:
    explicit MediaViewerDelegate(QAbstractItemModel* model,  //关联的数据模型，用于获取媒体文件信息
                                 int index,                  //媒体项在模型中的索引
                                 MediaViewer* window,        //媒体查看器窗口，用于显示媒体文件
                                 QObject* parent = nullptr); //可选的父对象，管理生命周期

    [[nodiscard]] auto getFilePath() const { return filepath; }        //返回当前媒体文件的路径
    [[nodiscard]] auto getImage() const { return this->image; }        //返回当前加载的图像
    void setScaleFactor(double newFactor) { scaleFactor = newFactor; } //设置图像的缩放因子
    void initConnections();                                            //链接
    void wheelEvent(QWheelEvent* event);                               //鼠标滚轮事件

signals:
    void imageChanged(bool fadeAnimation = true); //当图片发生变化时发出的信号
    void scaleFactorChanged(double newFactor);    //当缩放因子发生变化时发出的信号

public slots:
    void onModelRowsToBeRemoved(const QModelIndex& parent, int first, int last); //处理媒体项被删除
    bool copyImageToClipboard(); //将当前图像复制到剪切板
    void openImageFileDialog();  //打开文件对话框
    void saveImageFileDialog();
    void readFullInfo();
    void adaptiveResize();
    void deleteImage();
    void prevImage();
    void nextImage();
    void rotateImage();

private:
    QAbstractItemModel* mediaListModel;
    QPersistentModelIndex mediaIndex;
    QImage image;
    QString filepath;
    MediaViewer* view;
    QVBoxLayout* layout;
    double scaleFactor;

    bool loadImage(const QString& path, bool fadeAnimation = true);
    bool loadImage(const QImage& image, bool fadeAnimation = true);
    void scaleImage(double factor);
};
