#pragma once

#include "delegate/DiskScanner.h" //扫描和监控媒体文件
#include "model/MediaListModel.h"
#include "view/AboutPage.h"
#include "view/FavoritePage.h"
#include "view/GalleryPage.h"
#include "view/SettingPage.h"
#include <ElaWindow.h>
#include <QSortFilterProxyModel> //对数据模型进行排序和过滤

class MainWindow : public ElaWindow {
    Q_OBJECT //用于支持信号和槽机制

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

    void initWindow();  //初始化窗口的一些属性，窗口大小、标题
    void initContent(); //内容，页面
    void initModel();   //初始化数据模型，如创建媒体列表模型和排序过滤代理模型

private:
    // pages页面指针
    FavoritePage* favoritePage;
    GalleryPage* galleryPage;
    SettingPage* settingPage;
    AboutPage* aboutPage;

    // models模型指针
    MediaListModel* mediaModel; //媒体列表模型
    QSortFilterProxyModel* galleryModel;
    QSortFilterProxyModel* favoriteModel; //排序过滤

    // disk scanner磁盘扫描器
    DiskScanner* diskScanner; //媒体文件
};
