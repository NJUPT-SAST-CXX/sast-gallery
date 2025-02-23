#include "MediaViewerDelegate.h"
#include <ElaContentDialog.h>
#include <ElaMessageBar.h>
#include <ElaText.h>
#include <QApplication>
#include <QClipboard>
#include <QDesktopServices>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QGuiApplication>
#include <QImageReader>
#include <QPaintDevice>
#include <QProcess>
#include <QScreen>
#include <QUrl>
#include <QtConcurrent>
#include <model/MediaListModel.h>
#include <utils/Settings.hpp>
#include <utils/Tools.h>
#include <view/MediaViewer.h>

MediaViewerDelegate::MediaViewerDelegate(QAbstractItemModel* model,
                                         int index,
                                         MediaViewer* viewer,
                                         QObject* parent)
    : QObject(parent)
    , mediaListModel(model)
    , mediaIndex(model->index(index, MediaListModel::Path))
    , view(viewer) {
    filepath = mediaIndex.data().value<QString>();
    loadImage(filepath);
}

void MediaViewerDelegate::init() {
    updateLikeButtonState();
}

void MediaViewerDelegate::initConnections() {
    // connect to actions
    connect(mediaListModel,
            &QAbstractItemModel::rowsAboutToBeRemoved,
            this,
            &MediaViewerDelegate::onModelRowsToBeRemoved);

    connect(this, &MediaViewerDelegate::mediaChanged, this, &MediaViewerDelegate::onMediaChanged);

    connect(view->openFileAction, &QAction::triggered, this, [this]() {
        openImageFileDialog();
        if (this->image.isNull()) {
            ElaMessageBar::error(ElaMessageBarType::PositionPolicy::TopRight,
                                 "Error",
                                 "Image failed to load",
                                 3000,
                                 view->abstractMediaViewer);
        }
        loadImage(this->image);
        QScreen* screen = QGuiApplication::primaryScreen();
        QRect screenGeometry = screen->geometry();
        int screenWidth = screenGeometry.width();
        int screenHeight = screenGeometry.height();

        int windowWidth = screenWidth / 2;
        int windowHeight = screenHeight / 2;
        view->resize(windowWidth, windowHeight);

        // if you wanna adapt to your monitor resolution, use adaptiveResize();
    });
    connect(view->copyFileAction,
            &QAction::triggered,
            this,
            &MediaViewerDelegate::copyImageToClipboard);
    connect(view->saveFileAction,
            &QAction::triggered,
            this,
            &MediaViewerDelegate::saveImageFileDialog);
    connect(view->openInFileExplorerAction,
            &QAction::triggered,
            this,
            &MediaViewerDelegate::openInFileExplorer);

    connect(view->rotateAction, &QAction::triggered, this, &MediaViewerDelegate::rotateMedia);

    connect(view->deleteAction, &QAction::triggered, this, &MediaViewerDelegate::deleteMedia);

    connect(view->printAction, &QAction::triggered, this, [=]() {
        qDebug() << "Print action clicked";
    });

    connect(view->editAction, &QAction::triggered, this, [=]() {
        //TODO(optional): implement the edit functionality
    });

    connect(view->prevAction, &QAction::triggered, this, &MediaViewerDelegate::prevMedia);

    connect(view->nextAction, &QAction::triggered, this, &MediaViewerDelegate::nextMedia);

    connect(view->likeButton,
            &ElaIconButton::clicked,
            this,
            &MediaViewerDelegate::onLikeButtonClicked);

    connect(view->fileInfoButton,
            &ElaIconButton::clicked,
            this,
            &MediaViewerDelegate::onFileInfoClicked);

    connect(view->zoomInButton, &ElaIconButton::clicked, [this]() {
        scaleTo(getScale() + 10);
        view->zoomSlider->setValue(getScale());
    });

    connect(view->zoomSlider, &ElaSlider::valueChanged, [this](int value) { scaleTo(value); });

    connect(view->zoomOutButton, &ElaIconButton::clicked, [this]() {
        scaleTo(view->abstractMediaViewer->getScale() - 10);
        view->zoomSlider->setValue(getScale());
    });

    connect(view->maximizeButton, &ElaIconButton::clicked, [this]() {
        this->view->showMaximized();
    });

    connect(view->zoom2originalButton, &ElaIconButton::clicked, [this]() {
        scaleTo(100);
        view->zoomSlider->setValue(getScale());
    });

    connect(this, &MediaViewerDelegate::scaledByWheel, [this]() {
        view->zoomSlider->setValue(getScale());
    });

    connect(view->abstractMediaViewer,
            &AbstractMediaViewer::wheelScrolled,
            this,
            &MediaViewerDelegate::onWheelScrolled);
}

void MediaViewerDelegate::onModelRowsToBeRemoved(const QModelIndex& parent, int first, int last) {
    if (mediaIndex.row() >= first && mediaIndex.row() <= last) {
        if (last < mediaListModel->rowCount() - 1) {
            mediaIndex = mediaListModel->index(last + 1, MediaListModel::Path);
        } else if (first > 0) {
            mediaIndex = mediaListModel->index(first - 1, MediaListModel::Path);
        } else {
            view->close();
            return;
        }
        filepath = mediaIndex.data().value<QString>();
        loadImage(filepath);
    }
}

void MediaViewerDelegate::onMediaChanged(bool fadeAnimation) {
    updateLikeButtonState();
    view->abstractMediaViewer->setContent(image, fadeAnimation);
    view->fileInfoBriefText->setText(QString("%1 x %2 %3")
                                         .arg(QString::number(image.width()))
                                         .arg(QString::number(QImage(image).height()))
                                         .arg(Tools::fileSizeString(filepath)));
    view->setWindowTitle(QFileInfo(filepath).fileName());
    view->zoomSlider->setValue(view->abstractMediaViewer->getScale());
    view->fileInfoWidget->loadInfo(filepath);
}

void MediaViewerDelegate::onWheelScrolled(int delta) {
    if (settings.value("wheelBehavior").toInt() == 0) {
        const double scaleFactor = std::abs(delta) / 100.0;
        scaleTo(view->abstractMediaViewer->getScale() + delta / 10);
        emit scaledByWheel();
    } else {
        if (delta > 0) {
            prevMedia();
        } else {
            nextMedia();
        }
    }
}

bool MediaViewerDelegate::copyImageToClipboard() {
    if (this->image.isNull()) {
        ElaMessageBar::error(ElaMessageBarType::Bottom,
                             "Null Image!",
                             nullptr,
                             2000,
                             view->abstractMediaViewer);
        return false;
    } else {
        QClipboard* clipboard = QApplication::clipboard();
        clipboard->setImage(this->image);
        ElaMessageBar::success(ElaMessageBarType::Bottom,
                               "Copied!",
                               nullptr,
                               2000,
                               view->abstractMediaViewer);
        return true;
    }
}

void MediaViewerDelegate::openImageFileDialog() {
    filepath = QFileDialog::getOpenFileName(nullptr,
                                            "Choose Image File",
                                            "",
                                            "Image Files (*.png *.jpg *.bmp *.jpeg *.gif)");
    if (!filepath.isEmpty()) {
        loadImage(filepath);
    }
}

void MediaViewerDelegate::saveImageFileDialog() {
    filepath = QFileDialog::getSaveFileName(nullptr,
                                            "Save Image File",
                                            "",
                                            "Image Files (*.png *.jpg *.bmp *.jpeg *.gif)");
    if (!filepath.isEmpty()) {
        this->image.save(filepath);
    }
}

void MediaViewerDelegate::onLikeButtonClicked() {
    bool currentState = mediaListModel
                            ->data(
                                mediaListModel->index(mediaIndex.row(), MediaListModel::IsFavorite))
                            .toBool();
    mediaListModel->setData(mediaListModel->index(mediaIndex.row(), MediaListModel::IsFavorite),
                            !currentState,
                            Qt::EditRole);
    updateLikeButtonState();
}

void MediaViewerDelegate::onFileInfoClicked() {
    auto* fileInfoAnimation = new QPropertyAnimation(view->fileInfoWidget, "width");
    connect(fileInfoAnimation, &QPropertyAnimation::valueChanged, [=](const QVariant& value) {
        view->fileInfoWidget->setFixedWidth(value.toInt());
    });
    connect(fileInfoAnimation,
            &QPropertyAnimation::finished,
            fileInfoAnimation,
            &QObject::deleteLater);
    fileInfoAnimation->setEasingCurve(QEasingCurve::OutCubic);
    fileInfoAnimation->setDuration(150);
    if (view->fileInfoWidget->isVisible()) {
        fileInfoAnimation->setStartValue(view->fileInfoWidget->width());
        fileInfoAnimation->setEndValue(0);
        connect(fileInfoAnimation,
                &QPropertyAnimation::finished,
                view->fileInfoWidget,
                &QWidget::hide);
    } else {
        view->fileInfoWidget->show();
        fileInfoAnimation->setStartValue(view->fileInfoWidget->width());
        fileInfoAnimation->setEndValue(350);
    }
    fileInfoAnimation->start();
}

void MediaViewerDelegate::adaptiveResize() {
    // get the resolution of user's monitor
    QScreen* screen = QGuiApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    int screenWidth = screenGeometry.width();
    int screenHeight = screenGeometry.height();
    //get the size of the image

    QImage loadedImage = this->image;
    int imageWidth = loadedImage.width();
    int imageHeight = loadedImage.height();

    //calculate the size of the Window
    int windowWidth = imageWidth;
    int windowHeight = imageHeight;
    if (imageWidth > screenWidth || imageHeight > screenHeight) {
        qreal widthRatio = static_cast<qreal>(screenWidth) / windowWidth;
        qreal heightRatio = static_cast<qreal>(screenHeight) / windowHeight;
        qreal scaleFactor = qMin(widthRatio, heightRatio);
        windowWidth = static_cast<int>(windowWidth * scaleFactor);
        windowHeight = static_cast<int>(windowHeight * scaleFactor);
    }
    view->resize(windowWidth, windowHeight);
}

void MediaViewerDelegate::deleteMedia() {
    if (settings.value("confirmDeletion").toBool()) {
        // ask before deletion
        auto* confirmDialog = new ElaContentDialog(view);
        confirmDialog->setWindowTitle("Confirm Deletion");
        auto* centralWidget = new QWidget(view);
        auto* centralVLayout = new QVBoxLayout(centralWidget);
        centralVLayout->setContentsMargins(15, 25, 15, 10);
        auto* title = new ElaText("Delete File", view);
        title->setTextStyle(ElaTextType::Title);
        auto* subTitle = new ElaText("Are you sure you want to delete this file?", view);
        subTitle->setTextStyle(ElaTextType::Body);
        centralVLayout->addWidget(title);
        centralVLayout->addSpacing(2);
        centralVLayout->addWidget(subTitle);
        centralVLayout->addStretch();
        confirmDialog->setCentralWidget(centralWidget);
        confirmDialog->setRightButtonText("Yes");
        confirmDialog->setMiddleButtonText("No");
        confirmDialog->setLeftButtonText("Cancel");
        connect(confirmDialog, &ElaContentDialog::middleButtonClicked, this, [=, this]() {
            confirmDialog->close();
        });
        connect(confirmDialog, &ElaContentDialog::leftButtonClicked, this, [=, this]() {
            confirmDialog->close();
        });
        connect(confirmDialog, &ElaContentDialog::rightButtonClicked, this, [=, this]() {
            if (!QFile(filepath).remove()) {
                ElaMessageBar::error(ElaMessageBarType::Bottom,
                                     "Delete failed!",
                                     nullptr,
                                     2000,
                                     view->abstractMediaViewer);
            } else {
                ElaMessageBar::success(ElaMessageBarType::Bottom,
                                       "Deleted!",
                                       nullptr,
                                       2000,
                                       view->abstractMediaViewer);
            }
        });
        confirmDialog->exec();
    } else {
        if (!QFile(filepath).remove()) {
            ElaMessageBar::error(ElaMessageBarType::Bottom,
                                 "Delete failed!",
                                 nullptr,
                                 2000,
                                 view->abstractMediaViewer);
        } else {
            ElaMessageBar::success(ElaMessageBarType::Bottom,
                                   "Deleted!",
                                   nullptr,
                                   2000,
                                   view->abstractMediaViewer);
        }
    }
}

void MediaViewerDelegate::prevMedia() {
    if (mediaIndex.row() > 0) {
        mediaIndex = mediaListModel->index(mediaIndex.row() - 1, MediaListModel::Path);
    } else {
        mediaIndex = mediaListModel->index(mediaListModel->rowCount() - 1, MediaListModel::Path);
    }
    filepath = mediaIndex.data().value<QString>();
    loadMedia(filepath);
}

void MediaViewerDelegate::nextMedia() {
    if (mediaIndex.row() < mediaListModel->rowCount() - 1) {
        mediaIndex = mediaListModel->index(mediaIndex.row() + 1, MediaListModel::Path);
    } else {
        mediaIndex = mediaListModel->index(0, MediaListModel::Path);
    }
    filepath = mediaIndex.data().value<QString>();
    loadMedia(filepath);
}

void MediaViewerDelegate::rotateMedia() {
    QTransform transform;
    transform.rotate(90);
    loadImage(image.transformed(transform), false);
    auto _ [[maybe_unused]] = QtConcurrent::run([this]() { this->image.save(filepath); });
}

bool MediaViewerDelegate::loadImage(const QString& path, bool fadeAnimation) {
    if (path.isEmpty()) {
        return false;
    }
    try {
        QImage loaded(path);
        if (loaded.isNull()) {
            return false;
        }
        if (this->image.isNull() || this->image != loaded) {
            this->image = loaded;
            emit mediaChanged(fadeAnimation);
            return true;
        }
    } catch (...) {
    }
    return false;
}

// TODO: still need to implement this
bool MediaViewerDelegate::loadImage(const QImage& image, bool fadeAnimation) {
    if (image.isNull()) {
        return false;
    }
    try {
        if (this->image.isNull() || this->image != image) {
            this->image = image;
            emit mediaChanged(fadeAnimation);
            return true;
        }
    } catch (...) {
    }
    return false;
}

bool MediaViewerDelegate::loadVideo(const QString& path, bool fadeAnimation) {
    if (path.isEmpty()) {
        return false;
    }

    try {
        Media media(path);

        view->abstractMediaViewer = new VideoViewer(path, this->view);
        emit mediaChanged(fadeAnimation);
        return true;
    } catch (...) {
    }
    return false;
}

bool MediaViewerDelegate::loadMedia(const QString& path, bool fadeAnimation) {
    if (path.isEmpty()) {
        return false;
    }

    try {
        Media media(path);
        if (media.type == MediaType::Video) {
            return loadVideo(path, fadeAnimation);
        } else if (media.type == MediaType::Image) {
            return loadImage(path, fadeAnimation);
        }
    } catch (...) {
    }
    return false;
}

void MediaViewerDelegate::scaleTo(int percent) {
    view->abstractMediaViewer->scaleTo(percent);
}

int MediaViewerDelegate::getScale() const {
    return view->abstractMediaViewer->getScale();
}

void MediaViewerDelegate::openInFileExplorer() {
    QFileInfo fileInfo(filepath);
    if (fileInfo.exists()) {
        QString explorer = "explorer.exe"; // Only for Windows
        QStringList params;
        params << "/select," << QDir::toNativeSeparators(filepath);
        QProcess::startDetached(explorer, params);
    } else {
        ElaMessageBar::error(ElaMessageBarType::Bottom,
                             "File not found!",
                             nullptr,
                             2000,
                             view->abstractMediaViewer);
    }
}

void MediaViewerDelegate::updateLikeButtonState() {
    QModelIndex favoriteIndex = mediaListModel->index(mediaIndex.row(), MediaListModel::IsFavorite);

    bool isFavorite = mediaListModel->data(favoriteIndex).toBool();

    if (view && view->likeButton) {
        view->likeButton->setChecked(isFavorite);
        if (isFavorite) {
            // favorite state: normal is pink, hover is pinker
            view->likeButton->setLightIconColor(QColor("#FF4081"));
            view->likeButton->setDarkIconColor(QColor("#FF4081"));
            view->likeButton->setLightHoverIconColor(QColor("#E91E63"));
            view->likeButton->setDarkHoverIconColor(QColor("#E91E63"));
        } else {
            // not favorite state: normal mode as others
            view->likeButton->setLightIconColor(QColor("#000000"));
            view->likeButton->setDarkIconColor(QColor("#ffffff"));
            view->likeButton->setLightHoverIconColor(QColor("#000000"));
            view->likeButton->setDarkHoverIconColor(QColor("#ffffff"));
        }
    }
}