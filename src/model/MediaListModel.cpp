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
            saveFavorites();
        } else {
            isFavorite.remove(path.value(index.row()));
            dataChanged(index, index);
            saveFavorites();
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