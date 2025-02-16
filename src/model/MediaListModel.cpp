#include "MediaListModel.h"
#include <QDir>
#include <QDataStream>
#include <QFile>

// 修改为使用绝对路径
const QString MediaListModel::FAVORITES_FILE = QDir::current().absoluteFilePath("favorites.dat");

MediaListModel::MediaListModel(QObject* parent)
    : QAbstractTableModel(parent) {
    loadFavorites();
}

MediaListModel::~MediaListModel() {
    saveFavorites();
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
        if (value.value<bool>()) {
            isFavorite.insert(path.value(index.row()));
            dataChanged(index, index);
            return true;  // 修改：返回true表示数据已成功更新
        } else {
            isFavorite.remove(path.value(index.row()));
            dataChanged(index, index);
            return true;  // 修改：返回true表示数据已成功更新
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

void MediaListModel::saveFavorites() const {
    QFile file(FAVORITES_FILE);
    if (file.open(QIODevice::WriteOnly)) {
        QDataStream out(&file);
        out << isFavorite;
        qDebug() << "Favorites saved to:" << FAVORITES_FILE;
        qDebug() << "Current favorites:" << isFavorite;
    } else {
        qDebug() << "Failed to save favorites to:" << FAVORITES_FILE;
        qDebug() << "Error:" << file.errorString();
    }
}

void MediaListModel::loadFavorites() {
    QFile file(FAVORITES_FILE);
    if (file.open(QIODevice::ReadOnly)) {
        QDataStream in(&file);
        in >> isFavorite;
        qDebug() << "Loaded favorites from:" << FAVORITES_FILE;
        qDebug() << "Loaded favorites:" << isFavorite;
    } else {
        qDebug() << "No favorites file found or unable to open:" << FAVORITES_FILE;
    }
}

bool MediaListModel::toggleFavorite(const QString& filePath) {
    bool newState;
    if (isFavorite.contains(filePath)) {
        isFavorite.remove(filePath);
        newState = false;
    } else {
        isFavorite.insert(filePath);
        newState = true;
    }
    
    // 查找对应的行索引
    int row = path.indexOf(filePath);
    if (row >= 0) {
        QModelIndex idx = index(row, Property::IsFavorite);
        emit dataChanged(idx, idx, {Qt::DisplayRole, Qt::EditRole});
    }
    
    saveFavorites(); // 实时保存更改
    return newState;
}

bool MediaListModel::isFavoriteFile(const QString& path) const {
    return isFavorite.contains(path);
}