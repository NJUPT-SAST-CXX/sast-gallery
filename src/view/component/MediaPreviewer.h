#pragma once

#include <QAbstractItemModel>
#include <QDateTime>
#include <QEvent>
#include <QFutureWatcher>
#include <QLabel>
#include <QMediaPlayer>
#include <QPixmap>
#include <QVideoFrame>
#include <QVideoSink>
#include <QVideoWidget>
#include <model/Media.h>

// display media in thumbnail, supposed to be work with ImageFlexLayout
class MediaPreviewer : public QLabel {
    Q_OBJECT
public:
    MediaPreviewer(QAbstractItemModel* model, int rowIndex, QWidget* parent = nullptr);
    ~MediaPreviewer();

    // load image when show
    void paintEvent(QPaintEvent* event) override;

    QSize sizeHint() const override;

    void setPath(const QString& path);
    void setLastModifiedTime(const QDateTime& time);
    void setIsFavorite(bool isFavorite);
    QString path();
    QDateTime lastModifiedTime();
    bool isFavorite();

signals:
    void doubleClicked();

public slots:
    void loadImageComplete();
    void loadVideoComplete();

protected:
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;

private:
    Media media;
    QSize mediaSize;
    QPixmap originalPixmap;
    bool requireReloadImage;
    QFutureWatcher<QPixmap> imageLoadWatcher;
    QFutureWatcher<QPixmap> videoLoadWatcher;

    void initMedia();
    static QPixmap roundedPixmap(const QPixmap& original, double radius);
    QPixmap loadImage();
    QPixmap loadVideo();
    bool isVideoFile(const QString& path) const;

    void scaleAnimation(qreal startScale, qreal endScale, int duration = 100);
    QPixmap scalePixmapContent(qreal scaleFactor);
};