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
}

FavoritePage::~FavoritePage() {}