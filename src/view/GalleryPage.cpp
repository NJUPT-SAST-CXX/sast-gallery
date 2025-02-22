#include "GalleryPage.h"
#include "component/GalleryWidget.h"
#include <QListView>

GalleryPage::GalleryPage(QAbstractItemModel* model, QWidget* parent)
    : BasePage(parent) {
    setWindowTitle("Gallery");

    auto* centralWidget = new GalleryWidget(model, this);
    centralWidget->setWindowTitle("Gallery");

    addCentralWidget(centralWidget);
}

GalleryPage::~GalleryPage() {}
