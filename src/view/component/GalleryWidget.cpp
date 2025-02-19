#include "GalleryWidget.h"
#include "MediaPreviewer.h"       //预览单个媒体文件
#include "model/MediaListModel.h" //管理媒体文件列表

GalleryWidget::GalleryWidget(QAbstractItemModel* model, QWidget* parent)
    : mediaListModel(model) //用于存储媒体列表的数据模型
    , mediaLayout(new MediaFlexLayout{this})
    , QWidget(parent) {
    initModelSignals(); //初始化模型的信号链接
    resetPreviewers();  //重置预览器，根据模型中的数据创建相应的MediaPreviewer并添加到布局中
    setSizePolicy(QSizePolicy::Policy::Ignored, sizePolicy().verticalPolicy());
}

GalleryWidget::~GalleryWidget() = default;

QAbstractItemModel* GalleryWidget::model() {
    return mediaListModel;
} //访问模型

void GalleryWidget::setModel(QAbstractItemModel* model) {
    this->mediaListModel = model;
    initModelSignals();
    resetPreviewers();
} //设置模型

QLayout* GalleryWidget::layout(QLayout* layout) {
    return mediaLayout;
} //访问布局

void GalleryWidget::onModelDataChanged(const QModelIndex& topLeft,     //起始
                                       const QModelIndex& bottomRight, //结束
                                       const QList<int>& roles) {
    qDebug() << "onModelDataChanged\t" << topLeft << bottomRight;
    for (int row = topLeft.row(); row <= bottomRight.row(); row++) {
        for (int col = topLeft.column(); col <= bottomRight.column(); col++) { //遍历行列
            auto* item = mediaLayout->itemAt(row);                             //返回第row行的布局项
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
} //模型数据变化处理

void GalleryWidget::onModelModelReset() {
    qDebug() << "onModelModelReset";
    resetPreviewers();
} //模型重置处理函数

void GalleryWidget::onModelLayoutChanged(const QList<QPersistentModelIndex>& parents,
                                         QAbstractItemModel::LayoutChangeHint hint) {
    // require a new way to load image to reduce the cost of model reorder
    qDebug() << "onModelLayoutChanged";
    resetPreviewers();
} //模型

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