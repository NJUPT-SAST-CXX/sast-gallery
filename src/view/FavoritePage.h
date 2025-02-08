#pragma once

#include "../delegate/MediaViewerDelegate.h"
#include "BasePage.h"
#include "component/GalleryWidget.h"
#include <QAbstractItemModel>

class FavoritePage : public BasePage {
    Q_OBJECT

public:
    explicit FavoritePage(QAbstractItemModel* model, QWidget* parent = nullptr);
    ~FavoritePage();

public slots:
    void onAddToFavoritePage(const QString& filepath);

private:
    void updateFavoritePage();

    MediaViewerDelegate* delegate;
    GalleryWidget* galleryWidget;
    QStringList favoriteImages;
};
