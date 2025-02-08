#include "FavoritePage.h"
#include "../delegate/MediaViewerDelegate.h"
#include "../model/MediaListModel.h"
#include "component/GalleryWidget.h"
#include <ElaText.h>

FavoritePage::FavoritePage(QAbstractItemModel* model, QWidget* parent)
    : BasePage(parent) {
    setWindowTitle("Favorite");

    auto* centralWidget = new GalleryWidget(model);
    centralWidget->setWindowTitle("Favorite");

    addCentralWidget(centralWidget);

    // TODO: Implement delegate clarification
    //connect(delegate,&MediaViewerDelegate::addToFavoritePage,this,&FavoritePage::onAddToFavoritePage);
}

FavoritePage::~FavoritePage() {}

void FavoritePage::onAddToFavoritePage(const QString& filepath) {
    // Add the image to the FavoritePage
    favoriteImages.append(filepath);
    updateFavoritePage();
}

void FavoritePage::updateFavoritePage() {
    // Update the GalleryWidget with the favorite images
    MediaListModel* favoriteModel = new MediaListModel(this);
    favoriteModel->appendEntries(favoriteImages);

    // Update the GalleryWidget with the favorite images
    galleryWidget->setModel(favoriteModel);
}