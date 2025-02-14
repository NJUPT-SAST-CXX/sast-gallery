#include "FavoritePage.h"
#include "component/GalleryWidget.h"
#include "delegate/MediaViewerDelegate.h"
#include "model/MediaListModel.h"
#include <ElaText.h>

FavoritePage::FavoritePage(QAbstractItemModel* model, QWidget* parent)
    : BasePage(parent) {
    setWindowTitle("Favorites");

    centralWidget = new GalleryWidget(model);
    centralWidget->setWindowTitle("Favorites");

    addCentralWidget(centralWidget);

    auto* delegate = new MediaViewerDelegate(model, 0, nullptr, this);
    connect(delegate, &MediaViewerDelegate::addToFav, this, &FavoritePage::onAddToFav);
}

FavoritePage::~FavoritePage() {}

void FavoritePage::onAddToFav(const QString& path) {
    auto* model = centralWidget->model();
    auto index = model->index(model->rowCount() - 1, 0);
    model->setData(index, true, MediaListModel::IsFavorite);
    centralWidget->setModel(model);
}