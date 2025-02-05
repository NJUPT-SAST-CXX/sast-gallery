#pragma once

#include "delegate/DiskScanner.h"
#include "model/MediaListModel.h"
#include "view/AboutPage.h"
#include "view/FavoritePage.h"
#include "view/GalleryPage.h"
#include "view/SettingPage.h"
#include "view/component/MediaPreviewer.h"
#include <ElaWindow.h>
#include <QSortFilterProxyModel>

class MainWindow : public ElaWindow {
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

    void initWindow();
    void initContent();
    void initModel();

    /*public slots:
    void onFileModified(const QStringList& paths);
*/
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

    // previews
    //QList<MediaPreviewer*> previewers;
};