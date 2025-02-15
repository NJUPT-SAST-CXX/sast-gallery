#include "GalleryWidget.h"
#include "MediaPreviewer.h"
#include "model/MediaListModel.h"

GalleryWidget::GalleryWidget(QAbstractItemModel* model, QWidget* parent)
    : QWidget(parent)
    , mediaListModel(model)
    , mediaLayout(new MediaFlexLayout{this}) {
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
    for (int row = topLeft.row(); row <= bottomRight.row(); row++) {
        for (int col = topLeft.column(); col <= bottomRight.column(); col++) {
            auto* item = mediaLayout->itemAt(row);

            auto index = mediaListModel->index(row, col);

            auto data = mediaListModel->data(index);

            auto* previewer = dynamic_cast<MediaPreviewer*>(item->widget());

            switch (MediaListModel::Property(col)) {
            case MediaListModel::Path:
                previewer->setPath(data.value<QString>());
                break;
            case MediaListModel::LastModifiedTime:
                previewer->setLastModifiedTime(data.value<QDateTime>());
                break;
            case MediaListModel::IsFavorite:
                previewer->setIsFavorite(data.value<bool>());
                break;
            }
        }
    }
}

void GalleryWidget::onModelModelReset() {
    resetPreviewers();
}

void GalleryWidget::onModelLayoutChanged(const QList<QPersistentModelIndex>& parents,
                                         QAbstractItemModel::LayoutChangeHint hint) {
    resetPreviewers();
}

void GalleryWidget::onModelRowsMoved(const QModelIndex& sourceParent,
                                     int sourceStart,
                                     int sourceEnd,
                                     const QModelIndex& destinationParent,
                                     int destinationRow) {
    QList<QWidget*> movedList;
    for (int i = sourceStart; i <= sourceEnd; i++) {
        auto* item = mediaLayout->takeAt(sourceStart);
        if (!item) {
            qWarning() << "GalleryWidget::onModelRowsMoved - No item at index" << i;
            continue;
        }
        movedList += item->widget();
    }
    mediaLayout->insertWidgets(movedList, destinationRow);
}

void GalleryWidget::onModelRowsInserted(const QModelIndex& parent, int first, int last) {
    QList<QWidget*> insertList;
    for (int i = first; i <= last; i++) {
        auto* previewer = new MediaPreviewer(mediaListModel, i);
        insertList += previewer;
    }
    mediaLayout->insertWidgets(insertList, first);
}

void GalleryWidget::onModelRowsRemoved(const QModelIndex& parent, int first, int last) {
    int i = last - first + 1;
    while (i-- > 0) {
        auto* item = mediaLayout->takeAt(first);
        if (!item) {
            qWarning() << "GalleryWidget::onModelRowsRemoved - No item at index" << first;
            continue;
        }
        delete item->widget();
        delete item;
    }
}

void GalleryWidget::initModelSignals() {
    // disconnect old connections
    for (auto& connection : connections) {
        disconnect(connection);
    }

    // build new connections
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

    // verify connections
    for (const auto& connection : connections) {
        if (!connection) {
            qWarning()
                << "GalleryWidget::initModelSignals - Failed to establish one or more connections";
        }
    }
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