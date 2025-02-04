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
    explicit MediaViewerDelegate(QAbstractItemModel* model,
                                 int index,
                                 MediaViewer* viewer,
                                 QObject* parent = nullptr);

    [[nodiscard]] auto getFilePath() const { return filepath; }
    [[nodiscard]] auto getImage() const { return this->image; }
    void initConnections();

signals:
    void scaledByWheel();
    void imageChanged(bool fadeAnimation = true);

public slots:
    void onModelRowsToBeRemoved(const QModelIndex& parent, int first, int last);
    void onImageChanged(bool fadeAnimation = true);
    void onWheelScrolled(int delta);
    bool copyImageToClipboard();
    void openImageFileDialog();
    void saveImageFileDialog();
    void onFileInfoClicked();
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
    int scalePercent;

    bool loadImage(const QString& path, bool fadeAnimation = true);
    bool loadImage(const QImage& image, bool fadeAnimation = true);
    void scaleTo(int percent);
};