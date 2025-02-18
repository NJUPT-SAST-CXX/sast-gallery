#include "MainWindow.h"
#include "delegate/DiskScanner.h"
#include "model/MediaListModel.h" //媒体列表的数据模型
#include "utils/Settings.hpp"     //管理应用程序的设置

MainWindow::MainWindow(QWidget* parent)
    : ElaWindow(parent) { //传入父窗口指针
    initWindow();
    initModel();
    initContent();
    moveToCenter();
}

MainWindow::~MainWindow() {
    settings.setValue("windowSize", size()); //保存当前窗口大小

    delete galleryPage;
    delete favoritePage;
    delete aboutPage;
    delete settingPage;
    delete mediaModel;
    delete galleryModel;
    delete favoriteModel;
    delete diskScanner;
}

void MainWindow::initWindow() {
    resize(settings.value("windowSize").toSize()); //根据之前的保存恢复窗口大小
    setUserInfoCardVisible(false);                 //隐藏用户信息
    setWindowTitle("SAST Gallery");                //设置窗口标题

    setWindowButtonFlag(ElaAppBarType::ButtonType::StayTopButtonHint, false); //禁用总是置顶按钮
}

void MainWindow::initContent() {
    // pages
    galleryPage = new GalleryPage(galleryModel, this);
    addPageNode("Gallery", galleryPage, ElaIconType::Images);

    favoritePage = new FavoritePage(favoriteModel, this);
    addPageNode("Favourites", favoritePage, ElaIconType::Heart);

    aboutPage = new AboutPage(this);
    QString aboutPageKey;
    addFooterNode("About", aboutPage, aboutPageKey, 0, ElaIconType::CircleInfo);

    settingPage = new SettingPage(this);
    QString settingPageKey;
    addFooterNode("Setting", settingPage, settingPageKey, 0, ElaIconType::GearComplex);
}

void MainWindow::initModel() {
    mediaModel = new MediaListModel();

    galleryModel = new QSortFilterProxyModel();
    galleryModel->setSourceModel(mediaModel);             //将其源模型设置为mediamodel
    galleryModel->sort(MediaListModel::LastModifiedTime); //按照最后修改时间对媒体列表进行排序

    favoriteModel = new QSortFilterProxyModel();
    favoriteModel->setSourceModel(galleryModel);
    favoriteModel->setFilterKeyColumn(MediaListModel::IsFavorite); //过滤条件
    favoriteModel->setFilterFixedString("true");

    diskScanner = new DiskScanner; //扫描磁盘
    // clang-format off
    QObject::connect(diskScanner, &DiskScanner::fileCreated, mediaModel, &MediaListModel::appendEntries);//创建
    QObject::connect(diskScanner, &DiskScanner::fileDeleted, mediaModel, &MediaListModel::removeEntries);//删除
    QObject::connect(diskScanner, &DiskScanner::fileModified, mediaModel, &MediaListModel::modifiedEntries);//修改
    QObject::connect(diskScanner, &DiskScanner::fullScan, mediaModel, &MediaListModel::resetEntries);//全量扫描
    // clang-format on
    diskScanner->scan(); //启动扫描
}
