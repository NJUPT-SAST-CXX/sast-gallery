#include "FavoritePage.h"
#include "component/GalleryWidget.h"
#include "delegate/MediaViewerDelegate.h"
#include "model/MediaListModel.h"
#include <ElaText.h>

FavoritePage::FavoritePage(QAbstractItemModel* model, QWidget* parent)
    : BasePage(parent)
    , galleryWidget(new GalleryWidget(model, this)) { // 使用独立模型管理收藏

    galleryWidget->setWindowTitle("Favourites");

    // 初始化 delegate，使用传入的模型和初始索引 0（需确保模型有效性）
    delegate = new MediaViewerDelegate(model, 0, nullptr, this);

    // 连接信号：当 delegate 触发 addToFavoritePage 时，更新 FavoritePage
    connect(delegate,
            &MediaViewerDelegate::addToFavoritePage,
            this,
            &FavoritePage::onAddToFavoritePage);

    // 初始化 GalleryWidget 使用独立的收藏模型
    galleryWidget->setModel(model);
    addCentralWidget(galleryWidget);

    setWindowTitle("Favourites");
}

FavoritePage::~FavoritePage() {}

void FavoritePage::onAddToFavoritePage(const QString& filepath) {
    Q_UNUSED(filepath);
    // 只需刷新视图
    galleryWidget->resetPreviewers();
}