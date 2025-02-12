#include "MainWindow.h"
#include "delegate/DiskScanner.h"
#include "model/MediaListModel.h"
#include "utils/Settings.hpp"

MainWindow::MainWindow(QWidget* parent)
    : ElaWindow(parent){
    initWindow();
    initModel();
    initContent();
    moveToCenter();
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
    addPageNode("Gallery", galleryPage, ElaIconType::Images);

    favoritePage = new FavoritePage(favoriteModel, this);
    addPageNode("Favorites", favoritePage, ElaIconType::Heart);

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
    galleryModel->setSourceModel(mediaModel);
    galleryModel->sort(MediaListModel::LastModifiedTime);

    favoriteModel = new QSortFilterProxyModel();
    favoriteModel->setSourceModel(galleryModel);
    favoriteModel->setFilterKeyColumn(MediaListModel::IsFavorite);
    favoriteModel->setFilterFixedString("true");
    //loadModel(favoriteModel,"model.dat");

    diskScanner = new DiskScanner();
    // clang-format off
    QObject::connect(diskScanner, &DiskScanner::fileCreated, mediaModel, &MediaListModel::appendEntries);
    QObject::connect(diskScanner, &DiskScanner::fileDeleted, mediaModel, &MediaListModel::removeEntries);
    QObject::connect(diskScanner, &DiskScanner::fileModified, mediaModel, &MediaListModel::modifiedEntries);
    QObject::connect(diskScanner, &DiskScanner::fullScan, mediaModel, &MediaListModel::resetEntries);
    // clang-format on

    diskScanner->scan();
}

void MainWindow::saveModel(QAbstractItemModel *model, const QString fileName)
{
    QFile file(fileName);
    if(!file.open(QIODevice::WriteOnly))
    {
        qWarning()<<"无法打开文件进行写入操作";
            return;
    }
    QDataStream out(&file);
    for(int row=0;row < model->rowCount();++row){
        for(int col=0;col < model->columnCount();++col){
            QModelIndex index = model->index(row,col);
            out << row<<col<<model->data(index).toString();
        }
    }
    file.close();
}

void MainWindow::loadModel(QAbstractItemModel *model, const QString fileName)
{
    QFile file(fileName);
    if(!file.open(QIODevice::ReadOnly))
    {
        qWarning()<<"无法打开文件进行写入操作";
        return;
    }
    QDataStream in(&file);
    while (!in.atEnd())
    {
        int row,col;
        QString value;
        in >>row>>col>>value;
        QModelIndex index = model->index(row,col);
        model->setData(index,value);
    }
    file.close();
}
