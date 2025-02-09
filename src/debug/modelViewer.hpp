
#include "delegate/DiskScanner.h"
#include "model/MediaListModel.h"
#include "view/component/GalleryWidget.h"
#include <QAbstractProxyModel>
#include <QApplication>
#include <QLabel>
#include <QMainWindow>
#include <QScrollArea>
#include <QSortFilterProxyModel>
#include <QTableView>
#include <qboxlayout.h>
#include <qmainwindow.h>
#include <qnamespace.h>

// show 2 windows for models and `GalleryWidget`s
inline void modelViewer(MediaListModel* model,
                        DiskScanner* diskScanner,
                        QSortFilterProxyModel* galleryProxyModel,
                        QSortFilterProxyModel* favProxyModel) {
    DiskScanner& ds = *diskScanner;
    QSortFilterProxyModel& galleryP = *galleryProxyModel;
    QSortFilterProxyModel& favP = *favProxyModel;

    QObject::connect(&ds, &DiskScanner::fileCreated, model, &MediaListModel::appendEntries);
    QObject::connect(&ds, &DiskScanner::fileDeleted, model, &MediaListModel::removeEntries);
    QObject::connect(&ds, &DiskScanner::fileModified, model, &MediaListModel::modifiedEntries);
    QObject::connect(&ds, &DiskScanner::fullScan, model, &MediaListModel::resetEntries);

    auto* table1 = new QTableView();
    table1->setModel(model);

    auto* table2 = new QTableView();
    table2->setModel(&galleryP);

    auto* table3 = new QTableView();
    table3->setModel(&favP);

    auto* debugInfo = new QWidget();
    auto* debugLayout = new QVBoxLayout(debugInfo);
    debugLayout->addWidget(new QLabel("Raw"));
    debugLayout->addWidget(table1);
    debugLayout->addWidget(new QLabel("Gallery (sorted)"));
    debugLayout->addWidget(table2);
    debugLayout->addWidget(new QLabel("Favorite (sorted & filtered)"));
    debugLayout->addWidget(table3);
    table1->setColumnWidth(0, 500);
    table2->setColumnWidth(0, 500);
    table3->setColumnWidth(0, 500);
    table1->setColumnWidth(1, 200);
    table2->setColumnWidth(1, 200);
    table3->setColumnWidth(1, 200);

    QMainWindow* debugWin = new QMainWindow();
    debugWin->setCentralWidget(debugInfo);
    debugWin->show();

    auto* rawG = new GalleryWidget(model);
    auto* galG = new GalleryWidget(&galleryP);
    auto* favG = new GalleryWidget(&favP);

    auto* area1 = new QScrollArea();
    area1->setWidget(rawG);
    area1->setWidgetResizable(true);
    auto* area2 = new QScrollArea();
    area2->setWidget(galG);
    area2->setWidgetResizable(true);
    auto* area3 = new QScrollArea();
    area3->setWidget(favG);
    area3->setWidgetResizable(true);

    auto* view = new QWidget();
    auto* viewLayout = new QVBoxLayout(view);
    viewLayout->addWidget(new QLabel("Raw"));
    viewLayout->addWidget(area1, 1);
    viewLayout->addWidget(new QLabel("Gallery (sorted)"));
    viewLayout->addWidget(area2, 1);
    viewLayout->addWidget(new QLabel("Favorite (sorted & filtered)"));
    viewLayout->addWidget(area3, 1);

    QMainWindow* galleryWin = new QMainWindow();
    galleryWin->setCentralWidget(view);
    galleryWin->show();
}