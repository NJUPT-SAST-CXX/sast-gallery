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
#include <QPaintDevice>
#include <QScreen>
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
    , scalePercent(100)
    , view(viewer) {
    filepath = mediaIndex.data().value<QString>();
    loadImage(filepath);
}

void MediaViewerDelegate::initConnections() {
    // connect to actions
    connect(mediaListModel,
            &QAbstractItemModel::rowsAboutToBeRemoved,
            this,
            &MediaViewerDelegate::onModelRowsToBeRemoved);

    connect(this, &MediaViewerDelegate::imageChanged, this, &MediaViewerDelegate::onImageChanged);

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
    connect(view->saveFileAction,
            &QAction::triggered,
            this,
            &MediaViewerDelegate::saveImageFileDialog);

    //TODO(must): implement the openInFileExplorer functionality
    //connect(openInFileExplorerAction,......)

    connect(view->rotateAction, &QAction::triggered, this, &MediaViewerDelegate::rotateImage);

    connect(view->deleteAction, &QAction::triggered, this, &MediaViewerDelegate::deleteImage);

    connect(view->printAction, &QAction::triggered, this, [=]() {
        qDebug() << "Print action clicked";
    });

    connect(view->editAction, &QAction::triggered, this, [=]() {
        //TODO(optional): implement the edit functionality
    });

    connect(view->prevAction, &QAction::triggered, this, &MediaViewerDelegate::prevImage);

    connect(view->nextAction, &QAction::triggered, this, &MediaViewerDelegate::nextImage);

    connect(view->likeButton, &ElaIconButton::clicked, this, [=]() {
        //TODO(must): implement the like functionality
        // add the image to Favorite Page
    });

    connect(view->fileInfoButton, &ElaIconButton::clicked, this, &MediaViewerDelegate::readFullInfo);

    connect(view->zoomInButton, &ElaIconButton::clicked, [this]() {
        scaleTo(scalePercent + 10);
        view->zoomSlider->setValue(scalePercent);
    });

    connect(view->zoomSlider, &ElaSlider::valueChanged, [this](int value) { scaleTo(value); });

    connect(view->zoomOutButton, &ElaIconButton::clicked, [this]() {
        scaleTo(scalePercent - 10);
        view->zoomSlider->setValue(scalePercent);
    });

    connect(view->maximizeButton, &ElaIconButton::clicked, [this]() {
        this->view->showMaximized();
    });

    connect(view->zoom2originalButton, &ElaIconButton::clicked, [this]() {
        scaleTo(100);
        view->zoomSlider->setValue(scalePercent);
    });

    connect(this, &MediaViewerDelegate::scaledByWheel, [this]() {
        view->zoomSlider->setValue(scalePercent);
    });

    connect(view->imageViewer,
            &ImageViewer::wheelScrolled,
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

void MediaViewerDelegate::onImageChanged(bool fadeAnimation) {
    view->imageViewer->setContent(image, fadeAnimation);
    view->fileInfoBriefText->setText(QString("%1 x %2 %3")
                                         .arg(QString::number(image.width()))
                                         .arg(QString::number(QImage(image).height()))
                                         .arg(Tools::fileSizeString(filepath)));
    view->setWindowTitle(QFileInfo(filepath).fileName());
    scalePercent = 100;
    view->zoomSlider->setValue(scalePercent);
}

void MediaViewerDelegate::onWheelScrolled(int delta) {
    if (settings.value("wheelBehavior").toInt() == 0) {
        const double scaleFactor = std::abs(delta) / 100.0;
        scaleTo(scalePercent + delta / 10);
        emit scaledByWheel();
    } else {
        if (delta > 0) {
            prevImage();
        } else {
            nextImage();
        }
    }
}

bool MediaViewerDelegate::copyImageToClipboard() {
    if (this->image.isNull()) {
        ElaMessageBar::error(ElaMessageBarType::Bottom, "Null Image!", nullptr, 2000);
        return false;
    } else {
        QClipboard* clipboard = QApplication::clipboard();
        clipboard->setImage(this->image);
        ElaMessageBar::success(ElaMessageBarType::Bottom, "Copied!", nullptr, 2000);
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
    QImage image(filepath);
    QString fileInfo = QString("<html>"
                               "<head>"
                               "  <meta charset='utf-8'>"
                               "  <style type='text/css'>"
                               "    body {"
                               "      font-family: 'Microsoft YaHei', sans-serif;"
                               "      background-color:rgb(206, 236, 250);" // light blue
                               "      padding: 15px;"
                               "      margin: 0;"
                               "    }"
                               "    h2 {"
                               "      color: #0066cc;"
                               "      border-bottom: 2px solid #0066cc;"
                               "      padding-bottom: 5px;"
                               "      margin-bottom: 10px;"
                               "    }"
                               "    p {"
                               "      color: #333;"
                               "      font-size: 14px;"
                               "      margin: 8px 0;"
                               "    }"
                               "    strong {"
                               "      color: #000;"
                               "    }"
                               "  </style>"
                               "</head>"
                               "<body>"
                               "  <h2>Full File Info</h2>"
                               "  <p><strong>File Name:</strong> %1</p>"
                               "  <p><strong>File Resolution:</strong> %2 x %3</p>"
                               "  <p><strong>File Path:</strong> %4</p>"
                               "  <p><strong>File Size:</strong> %5</p>"
                               "  <p><strong>Created Time:</strong> %6</p>"
                               "  <p><strong>Modified Time:</strong> %7</p>"
                               "  <p><strong>Access Time:</strong> %8</p>"
                               "  <p><strong>File Type:</strong> %9</p>"
                               "</body>"
                               "</html>")
                           .arg(info.fileName())
                           .arg(image.width())
                           .arg(image.height())
                           .arg(info.absoluteFilePath())
                           .arg(Tools::fileSizeString(filepath))
                           .arg(info.birthTime().toString())
                           .arg(info.lastModified().toString())
                           .arg(info.lastRead().toString())
                           .arg(QString(QImageReader::imageFormat(filepath)).toUpper());

    ElaWidget* fileInfoWidget = new ElaWidget();
    fileInfoWidget->setIsStayTop(true);
    fileInfoWidget->setIsFixedSize(true);
    fileInfoWidget->setWindowTitle("Full Image Information");
    fileInfoWidget->setWindowButtonFlag(ElaAppBarType::ButtonType::StayTopButtonHint, false);
    fileInfoWidget->setStyleSheet("background-color: rgb(222, 241, 250);"); // light blue

    QLabel* label = new QLabel(fileInfoWidget);
    label->setTextFormat(Qt::RichText);
    label->setText(fileInfo);
    label->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    label->setMargin(10);

    QVBoxLayout* layout = new QVBoxLayout(fileInfoWidget);
    layout->addWidget(label);
    fileInfoWidget->setLayout(layout);

    fileInfoWidget->show();
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
            emit imageChanged(fadeAnimation);
            return true;
        }
    } catch (...) {
    }
    return false;
}

bool MediaViewerDelegate::loadImage(const QImage& image, bool fadeAnimation) {
    if (image.isNull()) {
        return false;
    }
    try {
        if (this->image.isNull() || this->image != image) {
            this->image = image;
            emit imageChanged(fadeAnimation);
            return true;
        }
    } catch (...) {
    }
    return false;
}

void MediaViewerDelegate::scaleTo(int percent) {
    if (percent < 1 || percent > 800) {
        qDebug() << "Invalid scale percent: " << percent;
        return;
    }
    const double scaleFactor = static_cast<double>(percent) / scalePercent;
    view->imageViewer->scale(scaleFactor, scaleFactor);
    scalePercent = percent;
}