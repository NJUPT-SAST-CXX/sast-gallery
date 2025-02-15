#include "MediaListModel.h"
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QTextStream>

MediaListModel::MediaListModel(QObject* parent)
    : QAbstractTableModel(parent) {
    QDir appDir = QDir::current();
    favoriteFilePath = appDir.absoluteFilePath("data/favorites.txt");
    // ensure the directory exists
    QDir dir(QFileInfo(favoriteFilePath).path());
    if (!dir.exists()) {
        dir.mkpath(".");
    }
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
            saveFavorites(); // save the changes
        } else {
            isFavorite.remove(path.value(index.row()));
            dataChanged(index, index);
            saveFavorites(); // save the changes
        }
        return true; // return true to indicate the data has been changed successfully
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
    qDebug() << "MediaListModel::modifiedEntries - Updating" << paths.size() << "files";

    for (auto& filePath : paths) {
        auto row = path.indexOf(filePath);
        if (row == -1) {
            qWarning() << "MediaListModel::modifiedEntries - File not found:" << filePath;
            continue;
        }

        QFileInfo fileInfo(filePath);
        if (!fileInfo.exists()) {
            qWarning() << "MediaListModel::modifiedEntries - File no longer exists:" << filePath;
            continue;
        }

        lastModifiedTime.replace(row, fileInfo.lastModified());
        // 发出所有相关列的变化信号
        emit dataChanged(index(row, Property::Path),
                         index(row, Property::LastModifiedTime),
                         {Qt::DisplayRole, Qt::EditRole});

        qDebug() << "MediaListModel::modifiedEntries - Updated row" << row
                 << "with new modification time:" << fileInfo.lastModified();
    }
}

void MediaListModel::loadFavorites() {
    QFile file(favoriteFilePath);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        while (!in.atEnd()) {
            QString line = in.readLine().trimmed();
            if (!line.isEmpty()) {
                isFavorite.insert(line);
            }
        }
        file.close();
    }
}

void MediaListModel::saveFavorites() {
    QFile file(favoriteFilePath);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        for (const QString& path : qAsConst(isFavorite)) {
            out << path << "\n";
        }
        file.close();
    }
}