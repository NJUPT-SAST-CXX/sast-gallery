#pragma once

#include "component/AbstractMediaViewer.h"
#include "component/FileInfoWidget.h"
#include "component/ImageViewer.h"
#include "component/VideoViewer.h"
#include <ElaIconButton.h>
#include <ElaSlider.h>
#include <ElaText.h>
#include <ElaWidget.h>
#include <QAbstractItemModel>
#include <QApplication>

class MediaViewerDelegate;

class MediaViewer : public ElaWidget {
    Q_OBJECT
    friend class MediaViewerDelegate;

public:
    explicit MediaViewer(QAbstractItemModel* model, int index, QWidget* parent = nullptr);
    ~MediaViewer();
    QSlider* getZoomSlider() const { return zoomSlider; }
    void initWindow();
    void initContent();

private:
    bool isFavorite = false;
    QAction* rotateAction{};
    QAction* deleteAction{};
    QAction* printAction{};
    QAction* editAction{};
    QAction* prevAction{};
    QAction* nextAction{};
    QAction* openFileAction{};
    QAction* copyFileAction{};
    QAction* saveFileAction{};
    QAction* openInFileExplorerAction{};
    ElaSlider* zoomSlider{};
    ElaIconButton* likeButton{};
    ElaIconButton* fileInfoButton{};
    ElaIconButton* zoomInButton{};
    ElaIconButton* zoomOutButton{};
    ElaIconButton* maximizeButton{};
    ElaIconButton* zoom2originalButton{};
    ElaText* fileInfoBriefText{};
    AbstractMediaViewer* abstractMediaViewer{nullptr}; // TODO: nullptr or not?
    // ImageViewer* imageViewer;
    // VideoViewer* videoViewer;
    FileInfoWidget* fileInfoWidget{};

    MediaViewerDelegate* delegate{};
};