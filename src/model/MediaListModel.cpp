#include "MediaListModel.h"
#include <QDir>

MediaListModel::MediaListModel(QObject* parent)
    : QAbstractTableModel(parent) {
    loadFavoriteStatus();
}

MediaListModel::~MediaListModel() {
    saveFavoriteStatus();
}

int MediaListModel::rowCount(const QModelIndex& parent) const {
    return path.size();
}

int MediaListModel::columnCount(const QModelIndex& parent) const {
    return 3;
}

QVariant MediaListModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid() || index.row() >= rowCount() || index.column() >= columnCount()) {
        return {};
    }

    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        switch (Property(index.column())) {
        case Property::Path:
            return path.value(index.row());
        case Property::LastModifiedTime:
            return lastModifiedTime.value(index.row());
        case Property::IsFavorite:
            return isFavorite.contains(path.value(index.row()));
        }
    }
    return {};
}

Qt::ItemFlags MediaListModel::flags(const QModelIndex& index) const {
    if (!index.isValid()) {
        return Qt::ItemIsEnabled;
    }
    switch (Property(index.column())) {
    case Property::Path:
    case Property::LastModifiedTime:
        return QAbstractTableModel::flags(index);
    case Property::IsFavorite:
        return QAbstractTableModel::flags(index) | Qt::ItemIsEditable;
    }
    return Qt::ItemIsEnabled;
}

QVariant MediaListModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (orientation != Qt::Horizontal || section >= columnCount()) {
        return {};
    }
    switch (Property(section)) {
    case Property::Path:
        return "Path";
    case Property::LastModifiedTime:
        return "LastModifiedTime";
    case Property::IsFavorite:
        return "IsFavorite";
    }
    return {};
}

bool MediaListModel::setData(const QModelIndex& index, const QVariant& value, int role) {
    if (!index.isValid() || index.row() >= rowCount() || index.column() != Property::IsFavorite) {
        return false;
    }
    if (role == Qt::EditRole) {
        QString currentPath = path.value(index.row());
        bool isCurrentlyFavorite = isFavorite.contains(currentPath);
        bool newFavoriteStatus = value.value<bool>();

        if (isCurrentlyFavorite != newFavoriteStatus) {
            if (newFavoriteStatus) {
                isFavorite.insert(currentPath);
            } else {
                isFavorite.remove(currentPath);
            }
            emit dataChanged(index, index); // 确保触发 dataChanged 信号
            saveFavoriteStatus();           // 保存收藏状态
            return true;
        }
    }
    return false;
}

void MediaListModel::resetEntries(const QStringList& paths) {
    beginResetModel();
    path = paths;
    lastModifiedTime.clear();
    for (auto& filePath : path) {
        lastModifiedTime += QFileInfo(filePath).lastModified();
    }
    endResetModel();
}

void MediaListModel::appendEntries(const QStringList& paths) {
    beginInsertRows({}, path.size(), path.size() + paths.size() - 1);
    path += paths;
    for (auto& filePath : paths) {
        lastModifiedTime += QFileInfo(filePath).lastModified();
    }
    endInsertRows();
}

void MediaListModel::removeEntries(const QStringList& paths) {
    for (auto& filePath : paths) {
        auto row = path.indexOf(filePath);
        beginRemoveRows({}, row, row);
        path.remove(row);
        lastModifiedTime.remove(row);
        endRemoveRows();
    }
}

void MediaListModel::modifiedEntries(const QStringList& paths) {
    for (auto& filePath : paths) {
        auto row = path.indexOf(filePath);
        lastModifiedTime.replace(row, QFileInfo(filePath).lastModified());
        dataChanged(index(row, Property::LastModifiedTime), index(row, Property::LastModifiedTime));
    }
}

void MediaListModel::loadFavoriteStatus() {
    QFile file("favoriteStatus.dat");
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Cannot open file for reading: " << file.errorString();
        return;
    }

    QDataStream in(&file);
    in.setVersion(QDataStream::Qt_5_0);

    // read favorite status
    in >> isFavorite;
}

void MediaListModel::saveFavoriteStatus() {
    QFile file("favoriteStatus.dat");
    if (!file.open(QIODevice::WriteOnly)) {
        qDebug() << "Cannot open file for writing: " << file.errorString();
        return;
    }

    QDataStream out(&file);
    out.setVersion(QDataStream::Qt_5_0);

    // write favorite status
    out << isFavorite;
}