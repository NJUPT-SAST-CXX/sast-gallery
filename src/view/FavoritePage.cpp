#include "FavoritePage.h"
#include "component/GalleryWidget.h"
#include <ElaText.h>

FavoritePage::FavoritePage(QAbstractItemModel* model, QWidget* parent)
    : BasePage(parent) {
    setWindowTitle("Favorite");

    auto* centralWidget = new GalleryWidget(model);
    centralWidget->setWindowTitle("Favorite");

    addCentralWidget(centralWidget);
}

FavoritePage::~FavoritePage() = default;
