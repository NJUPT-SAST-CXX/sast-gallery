#pragma once

#include "BasePage.h"
#include "component/GalleryWidget.h"
#include <QAbstractItemModel>

class FavoritePage : public BasePage {
    Q_OBJECT

public:
    explicit FavoritePage(QAbstractItemModel* model, QWidget* parent = nullptr);
    ~FavoritePage();

public slots:
    void onAddToFav(const QString& path);

private:
    GalleryWidget* centralWidget;
};