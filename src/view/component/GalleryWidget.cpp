#include "GalleryWidget.h"
#include "MediaPreviewer.h"
#include "model/MediaListModel.h"

GalleryWidget::GalleryWidget(QAbstractItemModel* model, QWidget* parent)
    : mediaListModel(model)
    , mediaLayout(new MediaFlexLayout{this})
    , QWidget(parent) {
    initModelSignals();
    resetPreviewers();
    setSizePolicy(QSizePolicy::Policy::Ignored, sizePolicy().verticalPolicy());
}

GalleryWidget::~GalleryWidget() {}

QAbstractItemModel* GalleryWidget::model() {
    return mediaListModel;
}

void GalleryWidget::setModel(QAbstractItemModel* model) {
    this->mediaListModel = model;
    initModelSignals();
    resetPreviewers();
}

QLayout* GalleryWidget::layout(QLayout* layout) {
    return mediaLayout;
}

void GalleryWidget::onModelDataChanged(const QModelIndex& topLeft,
                                       const QModelIndex& bottomRight,
                                       const QList<int>& roles) {
    qDebug() << "onModelDataChanged\t" << topLeft << bottomRight;
    for (int row = topLeft.row(); row <= bottomRight.row(); row++) {
        for (int col = topLeft.column(); col <= bottomRight.column(); col++) {
            auto* item = mediaLayout->itemAt(row);
            auto data = mediaListModel->data(mediaListModel->index(row, col));
            switch (MediaListModel::Property(col)) {
            case MediaListModel::Path:
                dynamic_cast<MediaPreviewer*>(item->widget())->setPath(data.value<QString>());
                break;
            case MediaListModel::LastModifiedTime:
                dynamic_cast<MediaPreviewer*>(item->widget())
                    ->setLastModifiedTime(data.value<QDateTime>());
                break;
            case MediaListModel::IsFavorite:
                dynamic_cast<MediaPreviewer*>(item->widget())->setIsFavorite(data.value<bool>());
                break;
            }
        }
    }
}

void GalleryWidget::onModelModelReset() {
    qDebug() << "onModelModelReset";
    resetPreviewers();
}

void GalleryWidget::onModelLayoutChanged(const QList<QPersistentModelIndex>& parents,
                                         QAbstractItemModel::LayoutChangeHint hint) {
    // require a new way to load image to reduce the cost of model reorder
    qDebug() << "onModelLayoutChanged";
    resetPreviewers();
}

void GalleryWidget::onModelRowsMoved(const QModelIndex& sourceParent,
                                     int sourceStart,
                                     int sourceEnd,
                                     const QModelIndex& destinationParent,
                                     int destinationRow) {
    qDebug() << "onModelRowsMoved" << sourceStart << sourceEnd << destinationRow;
    QList<QWidget*> movedList;
    for (int i = sourceStart; i <= sourceEnd; i++) {
        movedList += mediaLayout->takeAt(sourceStart)->widget();
    }
    mediaLayout->insertWidgets(movedList, destinationRow);
}

void GalleryWidget::onModelRowsInserted(const QModelIndex& parent, int first, int last) {
    qDebug() << "onModelRowsInserted" << first << last;

    QList<QWidget*> insertList;
    for (int i = first; i <= last; i++) {
        insertList += new MediaPreviewer(mediaListModel, i);
    }
    mediaLayout->insertWidgets(insertList, first);
}

void GalleryWidget::onModelRowsRemoved(const QModelIndex& parent, int first, int last) {
    qDebug() << "onModelRowsRemoved" << first << last;
    int i = last - first + 1;
    while (i-- > 0) {
        auto* item = mediaLayout->takeAt(first);
        delete item->widget();
        delete item;
    }
}

void GalleryWidget::initModelSignals() {
    static std::array<QMetaObject::Connection, 6> connections;

    for (auto& connection : connections) {
        disconnect(connection);
    }

    connections = {
        // clang-format off
    connect(mediaListModel, &QAbstractItemModel::dataChanged, this, &GalleryWidget::onModelDataChanged),
    connect(mediaListModel, &QAbstractItemModel::modelReset, this, &GalleryWidget::onModelModelReset),
    connect(mediaListModel, &QAbstractItemModel::layoutChanged, this, &GalleryWidget::onModelLayoutChanged),
    connect(mediaListModel, &QAbstractItemModel::rowsMoved, this, &GalleryWidget::onModelRowsMoved),
    connect(mediaListModel, &QAbstractItemModel::rowsInserted, this, &GalleryWidget::onModelRowsInserted),
    connect(mediaListModel, &QAbstractItemModel::rowsRemoved, this, &GalleryWidget::onModelRowsRemoved),
        // clang-format on
    };
}

void GalleryWidget::resetPreviewers() {
    auto row = mediaListModel->rowCount();
    delete mediaLayout;
    mediaLayout = new MediaFlexLayout{this};

    QList<QWidget*> widgets;
    for (int i = 0; i < row; i++) {
        widgets += new MediaPreviewer(mediaListModel, i);
    }
    mediaLayout->addWidgets(widgets);
}