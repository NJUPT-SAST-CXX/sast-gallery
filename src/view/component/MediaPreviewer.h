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
    explicit MediaPreviewer(QAbstractItemModel* model, int rowIndex, QWidget* parent = nullptr);
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

private:
    Media media;
    QSize mediaSize;
    bool requireReloadImage = true;
    QFutureWatcher<QPixmap> imageLoadWatcher;
    QFutureWatcher<QPixmap> videoLoadWatcher;

    QPixmap originalPixmap;

    void initMedia();
    static QPixmap roundedPixmap(const QPixmap& original, double radius);
    QPixmap loadImage();
    QPixmap loadVideo();

    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;

    // scale the content while keeping the geometry for layout stability
    QPixmap scalePixmapContent(qreal scaleFactor);

    void scaleAnimation(qreal startScale, qreal endScale, int duration = 200);
};