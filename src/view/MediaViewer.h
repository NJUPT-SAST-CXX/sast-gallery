#pragma once

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
    ~MediaViewer() override;
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
    MediaViewerDelegate* delegate{};

    // Content widgets
    QWidget* contentWidget{nullptr}; // Points to either imageViewer or videoViewer
    ImageViewer* imageViewer{nullptr};
    VideoViewer* videoViewer{nullptr};
    FileInfoWidget* fileInfoWidget{};
};