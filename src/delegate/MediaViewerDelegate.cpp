#include "MediaViewerDelegate.h"
#include <ElaContentDialog.h>
#include <ElaMessageBar.h>
#include <ElaText.h>
#include <QApplication>
#include <QClipboard>
#include <QFile>
#include <QFileDialog>
#include <QGuiApplication>
#include <QImageReader>
#include <QMessageBox>
#include <QPaintDevice>
#include <QScreen>
#include <QtConcurrent>
#include <model/MediaListModel.h>
#include <utils/Settings.hpp>
#include <utils/Tools.h>
#include <view/MediaViewer.h>

MediaViewerDelegate::MediaViewerDelegate(QAbstractItemModel* model,
                                         int index,
                                         MediaViewer* view,
                                         QObject* parent)
    : QObject(parent)
    , mediaListModel(model)
    , mediaIndex(model->index(index, MediaListModel::Path))
    , scaleFactor(1.0)
    , view(view) {
    filepath = mediaIndex.data().value<QString>();
    loadImage(filepath);
    connect(mediaListModel,
            &QAbstractItemModel::rowsAboutToBeRemoved,
            this,
            &MediaViewerDelegate::onModelRowsToBeRemoved);
}

void MediaViewerDelegate::initConnections() {
    // connect to actions
    connect(view->openFileAction, &QAction::triggered, this, [this]() {
        openImageFileDialog();
        if (this->image.isNull()) {
            ElaMessageBar::error(ElaMessageBarType::PositionPolicy::TopRight,
                                 "Error",
                                 "Image failed to load",
                                 3000,
                                 view);
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
    connect(view->saveasFileAction,
            &QAction::triggered,
            this,
            &MediaViewerDelegate::saveImageFileDialog);

    //TODO(must):implement the openInFileExplorer functionality
    //connect(openInFileExplorerAction,......)

    connect(view->rotateAction, &QAction::triggered, this, &MediaViewerDelegate::rotateImage);

    connect(view->deleteAction, &QAction::triggered, this, &MediaViewerDelegate::deleteImage);

    connect(view->printAction, &QAction::triggered, this, [=]() {
        qDebug() << "Print action clicked";
    });

    connect(view->editAction, &QAction::triggered, this, [=]() {
        //TODO(optional):implement the edit functionality
    });

    connect(view->prevAction, &QAction::triggered, this, &MediaViewerDelegate::prevImage);

    connect(view->nextAction, &QAction::triggered, this, &MediaViewerDelegate::nextImage);

    connect(view->likeButton, &ElaIconButton::clicked, this, [=]() {
        //TODO(must):implement the like functionality
        // add the image to Favorite Page
    });

    connect(view->fileInfoButton, &ElaIconButton::clicked, this, &MediaViewerDelegate::readFullInfo);

    connect(view->zoomInButton, &ElaIconButton::clicked, this, [this]() {
        scaleFactor += 0.05; //需改为显示捕获
        if (scaleFactor > 3)
            scaleFactor = 3;
        scaleImage(scaleFactor);
        view->zoomSlider->setToolTip(QString::number(scaleFactor * 100));
        view->zoomSlider->setValue(scaleFactor * 100);
    });

    connect(view->zoomOutButton, &ElaIconButton::clicked, this, [this]() {
        scaleFactor -= 0.05;
        if (scaleFactor < 0.2)
            scaleFactor = 0.1;
        scaleImage(scaleFactor);
        view->zoomSlider->setToolTip(QString::number(scaleFactor * 100));
        view->zoomSlider->setValue(scaleFactor * 100);
    });

    connect(view->maximizeButton, &ElaIconButton::clicked, this, [this]() {
        this->view->showMaximized();
    });

    connect(view->zoom2originalButton, &ElaIconButton::clicked, this, [this]() {
        scaleImage(1.0);
        view->zoomSlider->setToolTip(QString::number(scaleFactor * 100));
        view->zoomSlider->setValue(scaleFactor * 100);
    });

    connect(view->zoomSlider, &ElaSlider::valueChanged, this, [this](int value) {
        // range from 1% to 300%
        if (value >= 1 && value <= 300) {
            scaleImage(value / 100.0);
            view->zoomSlider->setToolTip(QString::number(value));
            view->zoomSlider->setValue(value);
        }
    });

    connect(this, &MediaViewerDelegate::imageChanged, this, [this](bool fadeAnimation) {
        view->imageViewer->setContent(image, fadeAnimation);
        view->fileInfoBriefText->setText(QString("%1 x %2 %3")
                                             .arg(QString::number(image.width()))
                                             .arg(QString::number(QImage(image).height()))
                                             .arg(Tools::fileSizeString(filepath)));
        view->setWindowTitle(QFileInfo(filepath).fileName());
    });
    connect(view->imageViewer, &ImageViewer::scaleFactorChanged, this, [this](double newFactor) {
        setScaleFactor(newFactor);
        view->zoomSlider->setValue(newFactor * 100);
    });
}

void MediaViewerDelegate::wheelEvent(QWheelEvent* event) {
    if (settings.value("wheelBehavior").toInt() == 0) {
        view->imageViewer->setWheelZoom(true);
    } else {
        view->imageViewer->setWheelZoom(false);
        if (event->angleDelta().y() > 0) {
            prevImage();
        } else {
            nextImage();
        }
    }
    event->accept();
}

void MediaViewerDelegate::onModelRowsToBeRemoved(const QModelIndex& parent, int first, int last) {
    // check if the current image is deleted
    if (mediaIndex.row() >= first && mediaIndex.row() <= last) {
        /* 
            current image is deleted, load the nearest image
            check if there is any image after the current image, 
            if not, check if there is any image before the current image
        */
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

bool MediaViewerDelegate::copyImageToClipboard() {
    if (this->image.isNull()) {
        QClipboard* clipboard = QApplication::clipboard();
        clipboard->setText("Image failed to copy to clipboard,plz load image first");
        return false;
    } else {
        QClipboard* clipboard = QApplication::clipboard();
        clipboard->setImage(this->image);
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

void MediaViewerDelegate::readFullInfo() {
    QFileInfo info(filepath);
    QString fileInfo = "File Name: " + info.fileName() + "\n";
    fileInfo += "File Resolution: " + QString::number(QImage(filepath).width()) + "x"
                + QString::number(QImage(filepath).height()) + "\n";
    fileInfo += "File Path: " + info.absoluteFilePath() + "\n";
    fileInfo += "File Size: " + Tools::fileSizeString(filepath) + "\n";
    fileInfo += "File Created: " + info.birthTime().toString() + "\n";
    fileInfo += "File Modified: " + info.lastModified().toString() + "\n";
    fileInfo += "File Accessed: " + info.lastRead().toString() + "\n";
    fileInfo += "File Type: " + QImageReader::imageFormat(filepath).toUpper() + "\n";

    QMessageBox::information(view, "Full Image Information", fileInfo);
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

void MediaViewerDelegate::deleteImage() {
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
        connect(confirmDialog, &ElaContentDialog::rightButtonClicked, this, [=, this]() {
            if (!QFile(filepath).remove()) {
                ElaMessageBar::error(ElaMessageBarType::Bottom, "Delete failed!", nullptr, 2000);
            } else {
                ElaMessageBar::success(ElaMessageBarType::Bottom, "Deleted!", nullptr, 2000);
            }
        });
        confirmDialog->exec();
    } else {
        if (!QFile(filepath).remove()) {
            ElaMessageBar::error(ElaMessageBarType::Bottom, "Delete failed!", nullptr, 2000);
        } else {
            ElaMessageBar::success(ElaMessageBarType::Bottom, "Deleted!", nullptr, 2000);
        }
    }
}

void MediaViewerDelegate::prevImage() {
    if (mediaIndex.row() > 0) {
        mediaIndex = mediaListModel->index(mediaIndex.row() - 1, MediaListModel::Path);
    } else {
        mediaIndex = mediaListModel->index(mediaListModel->rowCount() - 1, MediaListModel::Path);
    }
    filepath = mediaIndex.data().value<QString>();
    loadImage(filepath);
}

void MediaViewerDelegate::nextImage() {
    if (mediaIndex.row() < mediaListModel->rowCount() - 1) {
        mediaIndex = mediaListModel->index(mediaIndex.row() + 1, MediaListModel::Path);
    } else {
        mediaIndex = mediaListModel->index(0, MediaListModel::Path);
    }
    filepath = mediaIndex.data().value<QString>();
    loadImage(filepath);
}

void MediaViewerDelegate::rotateImage() {
    QTransform transform;
    transform.rotate(90);
    loadImage(image.transformed(transform), false);
    QFuture<void> future = QtConcurrent::run([this]() { this->image.save(filepath); });
}

bool MediaViewerDelegate::loadImage(const QString& path, bool fadeAnimation) {
    try {
        if (path.isEmpty()) {
            return false;
        }
        QImage loaded(path);
        if (loaded.isNull()) {
            return false;
        }
        if (this->image.isNull() || this->image != loaded) {
            this->image = loaded;
            emit imageChanged(fadeAnimation);
            return true;
        }
        return false;
    } catch (...) {
        return false;
    }
}

bool MediaViewerDelegate::loadImage(const QImage& image, bool fadeAnimation) {
    try {
        if (image.isNull()) {
            return false;
        }
        if (this->image.isNull() || this->image != image) {
            this->image = image;
            emit imageChanged(fadeAnimation);
            return true;
        }
        return false;
    } catch (...) {
        return false;
    }
}

void MediaViewerDelegate::scaleImage(double factor) {
    scaleFactor = factor;
    view->imageViewer->scale(scaleFactor, scaleFactor);
}