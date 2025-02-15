#include "MainWindow.h"
#include "delegate/DiskScanner.h"
#include "model/MediaListModel.h"
#include "utils/Settings.hpp"
#include <QDataStream>
#include <QFile>

MainWindow::MainWindow(QWidget* parent)
    : ElaWindow(parent) {
    initWindow();
    initModel();
    initContent();
    moveToCenter();
    loadFavourite();
}

MainWindow::~MainWindow() {
    settings.setValue("windowSize", size());
}

void MainWindow::initWindow() {
    resize(settings.value("windowSize").toSize());
    setUserInfoCardVisible(false);
    setWindowTitle("SAST Gallery");

    setWindowButtonFlag(ElaAppBarType::ButtonType::StayTopButtonHint, false);
}

void MainWindow::initContent() {
    // pages
    galleryPage = new GalleryPage(galleryModel, this);
    addPageNode("相册", galleryPage, ElaIconType::Images);

    favoritePage = new FavoritePage(favoriteModel, this);
    addPageNode("喜欢", favoritePage, ElaIconType::Heart);

    aboutPage = new AboutPage(this);
    QString aboutPageKey;
    addFooterNode("关于", aboutPage, aboutPageKey, 0, ElaIconType::CircleInfo);

    settingPage = new SettingPage(this);
    QString settingPageKey;
    addFooterNode("设置", settingPage, settingPageKey, 0, ElaIconType::GearComplex);
}

void MainWindow::initModel() {
    mediaModel = new MediaListModel();

    galleryModel = new QSortFilterProxyModel();
    galleryModel->setSourceModel(mediaModel);
    galleryModel->sort(MediaListModel::LastModifiedTime);

    favoriteModel = new QSortFilterProxyModel();
    // favoriteModel->setSourceModel(galleryModel);
    favoriteModel->setSourceModel(mediaModel);
    favoriteModel->setFilterKeyColumn(MediaListModel::IsFavorite);
    favoriteModel->setFilterFixedString("true");
    favoriteModel->sort(MediaListModel::LastModifiedTime);

    diskScanner = new DiskScanner();
    // clang-format off
    QObject::connect(diskScanner, &DiskScanner::fileCreated, mediaModel, &MediaListModel::appendEntries);
    QObject::connect(diskScanner, &DiskScanner::fileDeleted, mediaModel, &MediaListModel::removeEntries);
    QObject::connect(diskScanner, &DiskScanner::fileModified, mediaModel, &MediaListModel::modifiedEntries);
    QObject::connect(diskScanner, &DiskScanner::fullScan, mediaModel, &MediaListModel::resetEntries);
    // clang-format on

    diskScanner->scan();

}
//
void MainWindow::loadFavourite(){
    QFile file("favourite.data");
    if(file.open(QIODevice::ReadOnly)){
        QSet<QString> favourite;
        QDataStream in(&file);
        in>>favourite;
        for(auto p:favourite){
            if(mediaModel->getPath().contains(p)){
                auto row=mediaModel->getPath().indexOf(p);
                mediaModel->setData(mediaModel->index(row,MediaListModel::IsFavorite),p,Qt::EditRole);
            }
        }
        file.close();
    }
}
