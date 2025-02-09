#include "GalleryWidget.h"
#include "MediaPreviewer.h"
#include "model/MediaListModel.h"
#include "view/FavoritePage.h"
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

    // 仅处理 IsFavorite 列的变化
    if (topLeft.column() != MediaListModel::IsFavorite
        && bottomRight.column() != MediaListModel::IsFavorite) {
        return;
    }

    for (int row = topLeft.row(); row <= bottomRight.row(); row++) {
        QModelIndex index = mediaListModel->index(row, MediaListModel::IsFavorite);
        bool isFavorite = index.data().toBool();

        // 如果是收藏状态变化，且当前模型是收藏模型，则直接更新对应项
        if (auto* previewer = dynamic_cast<MediaPreviewer*>(mediaLayout->itemAt(row)->widget())) {
            previewer->setIsFavorite(isFavorite);
            // 如果当前是收藏页且取消收藏，移除该预览器
            if (dynamic_cast<FavoritePage*>(parent()) && !isFavorite) {
                mediaLayout->removeWidget(previewer);
                previewer->deleteLater();
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
    qFatal("onModelLayoutChanged called");
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
    // clang-format off
    connect(mediaListModel, &QAbstractItemModel::dataChanged, this, &GalleryWidget::onModelDataChanged);
    connect(mediaListModel, &QAbstractItemModel::modelReset, this, &GalleryWidget::onModelModelReset);
    connect(mediaListModel, &QAbstractItemModel::layoutChanged, this, &GalleryWidget::onModelLayoutChanged);
    connect(mediaListModel, &QAbstractItemModel::rowsMoved, this, &GalleryWidget::onModelRowsMoved);
    connect(mediaListModel, &QAbstractItemModel::rowsInserted, this, &GalleryWidget::onModelRowsInserted);
    connect(mediaListModel, &QAbstractItemModel::rowsRemoved, this, &GalleryWidget::onModelRowsRemoved);
    // clang-format on
}

void GalleryWidget::resetPreviewers() {
    auto row = mediaListModel->rowCount();
    delete mediaLayout;
    mediaLayout = new MediaFlexLayout{this};

    QList<QWidget*> widgets;
    QSet<QString> existingPaths; // 用于存储已经存在的路径

    for (int i = 0; i < row; i++) {
        QString filepath = mediaListModel->data(mediaListModel->index(i, MediaListModel::Path))
                               .toString();
        if (!existingPaths.contains(filepath)) {
            widgets += new MediaPreviewer(mediaListModel, i);
            existingPaths.insert(filepath); // 将路径添加到集合中
        }
    }
    mediaLayout->addWidgets(widgets);
}