#pragma once
#include "MediaFlexLayout.h"
#include <QAbstractItemModel>
#include <QWidget>
#include <vector>

class GalleryWidget : public QWidget {
    Q_OBJECT

    MediaFlexLayout* mediaLayout;
    QAbstractItemModel* mediaListModel;

public:
    GalleryWidget(QAbstractItemModel* model, QWidget* parent = nullptr);
    ~GalleryWidget();

    QAbstractItemModel* model();
    void setModel(QAbstractItemModel* model);
    QLayout* layout(QLayout* layout);

public slots:

    void onModelDataChanged(const QModelIndex& topLeft,
                            const QModelIndex& bottomRight,
                            const QList<int>& roles = QList<int>());
    void onModelModelReset();
    void onModelLayoutChanged(
        const QList<QPersistentModelIndex>& parents = QList<QPersistentModelIndex>(),
        QAbstractItemModel::LayoutChangeHint hint = QAbstractItemModel::NoLayoutChangeHint);
    void onModelRowsMoved(const QModelIndex& sourceParent,
                          int sourceStart,
                          int sourceEnd,
                          const QModelIndex& destinationParent,
                          int destinationRow);
    void onModelRowsInserted(const QModelIndex& parent, int first, int last);
    void onModelRowsRemoved(const QModelIndex& parent, int first, int last);

private:
    std::vector<QMetaObject::Connection> connections;
    void initModelSignals();
    void resetPreviewers();
};