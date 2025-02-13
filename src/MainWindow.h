#pragma once

#include "delegate/DiskScanner.h"
#include "model/MediaListModel.h"
#include "view/AboutPage.h"
#include "view/FavoritePage.h"
#include "view/GalleryPage.h"
#include "view/SettingPage.h"
#include <ElaWindow.h>
#include <QSortFilterProxyModel>
#include <QFile>
#include <qcoreapplication.h>

class MainWindow : public ElaWindow  {
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

    void initWindow();
    void initContent();
    void initModel();


private:
    // pages
    FavoritePage* favoritePage;
    GalleryPage* galleryPage;
    SettingPage* settingPage;
    AboutPage* aboutPage;

    // models
    MediaListModel* mediaModel;
    QSortFilterProxyModel* galleryModel;
    QSortFilterProxyModel* favoriteModel;

    // disk scanner
    DiskScanner* diskScanner;
};
