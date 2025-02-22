#include "MediaListModel.h"
#include <QDir>
#include <QStandardPaths>

MediaListModel::MediaListModel(QObject* parent)
    : QAbstractTableModel(parent) {
    loadFavorites();
}

MediaListModel::~MediaListModel() {
    saveFavorites();
}

int MediaListModel::rowCount(const QModelIndex& parent) const {
    return path.size();
} //行数统计

int MediaListModel::columnCount(const QModelIndex& parent) const {
    return 3;
} //列数固定为3列

QVariant MediaListModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid() || index.row() >= rowCount() || index.column() >= columnCount()) {
        return {};
    } //检查索引是否有效

    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        switch (Property(index.column())) {
        case Property::Path:
            return path.value(index.row()); //返回对应行的路径
        case Property::LastModifiedTime:
            return lastModifiedTime.value(index.row());
        case Property::IsFavorite:
            return isFavorite.contains(path.value(index.row())); //返回的是是否包含
        }
    }
    return {};
} //相应数据

Qt::ItemFlags MediaListModel::flags(const QModelIndex& index) const {
    if (!index.isValid()) {
        return Qt::ItemIsEnabled;
    } //如果索引无效
    switch (Property(index.column())) {
    case Property::Path:
    case Property::LastModifiedTime:
        return QAbstractTableModel::flags(index);
    case Property::IsFavorite:
        return QAbstractTableModel::flags(index) | Qt::ItemIsEditable; //表示可编辑
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
} //表头数据

bool MediaListModel::setData(const QModelIndex& index, const QVariant& value, int role) {
    if (!index.isValid() || index.row() >= rowCount() || index.column() != Property::IsFavorite) {
        return false;
    }
    if (role == Qt::EditRole) {
        if (value.value<bool>()) {
            isFavorite.insert(path.value(index.row())); //将路径添加到iF中，并发出信号
            dataChanged(index, index);
        } else {
            isFavorite.remove(path.value(index.row())); //移除路径
            dataChanged(index, index);
        }
        saveFavorites(); // 保存收藏状态
        return true;
    }
    return false;
} //设置数据

void MediaListModel::resetEntries(const QStringList& paths) {
    beginResetModel();        //开始重构模型
    path = paths;             //更新列表
    lastModifiedTime.clear(); //清空列表
    for (auto& filePath : path) {
        lastModifiedTime += QFileInfo(filePath).lastModified();
    }
    endResetModel();
} //重置模型中的条目

void MediaListModel::appendEntries(const QStringList& paths) {
    beginInsertRows({}, path.size(), path.size() + paths.size() - 1); //开始并指定插入行
    path += paths;                                                    //追加
    for (auto& filePath : paths) {
        lastModifiedTime += QFileInfo(filePath).lastModified();
    } //计算最后修改时间
    endInsertRows();
}

void MediaListModel::removeEntries(const QStringList& paths) {
    for (auto& filePath : paths) {
        auto row = path.indexOf(filePath); //找到行号
        beginRemoveRows({}, row, row);
        path.remove(row);
        lastModifiedTime.remove(row);
        endRemoveRows();
    }
} //删除条目

void MediaListModel::modifiedEntries(const QStringList& paths) {
    for (auto& filePath : paths) {
        auto row = path.indexOf(filePath);
        lastModifiedTime.replace(row, QFileInfo(filePath).lastModified());
        dataChanged(index(row, Property::LastModifiedTime),
                    index(row, Property::LastModifiedTime)); //发送信号
    }
} //修改条目

void MediaListModel::saveFavorites() {
    QString filePath = getFavoritesFilePath();

    QFile file(filePath);

    qDebug() << "Saving favorites to:" << filePath;
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Failed to open favorites file for writing:" << file.errorString();
        qDebug() << "File exists:" << QFile::exists(filePath);
        qDebug() << "File permissions:" << file.permissions();
        return;
    }

    QDataStream out(&file);
    out << isFavorite;
    file.close();
}

void MediaListModel::loadFavorites() {
    QFile file(getFavoritesFilePath());
    if (file.open(QIODevice::ReadOnly)) {
        QDataStream in(&file);
        in >> isFavorite;
        file.close();
    }
}
QString MediaListModel::getFavoritesFilePath() {
    QString dataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(dataDir);
    return dataDir + "/favorites.dat";
}
