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
#include <QProcess>
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
    , view(viewer) {                               //初始化
    filepath = mediaIndex.data().value<QString>(); //获取文件路径
    loadImage(filepath);                           //加载该文件
}

void MediaViewerDelegate::initConnections() {
    // connect to actions
    connect(mediaListModel,
            &QAbstractItemModel::rowsAboutToBeRemoved,
            this,
            &MediaViewerDelegate::onModelRowsToBeRemoved); //媒体项被移除

    connect(this,
            &MediaViewerDelegate::imageChanged,
            this,
            &MediaViewerDelegate::onImageChanged); //更新页面显示

    connect(view->openFileAction, &QAction::triggered, this, [this]() {
        openImageFileDialog();
        if (this->image.isNull()) {
            ElaMessageBar::error(ElaMessageBarType::PositionPolicy::TopRight,
                                 "Error",
                                 "Image failed to load",
                                 3000,
                                 view->imageViewer);
        } //检查是否成功加载图像
        loadImage(this->image); //加载图像
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
            &MediaViewerDelegate::copyImageToClipboard); //复制图片
    connect(view->saveFileAction,
            &QAction::triggered,
            this,
            &MediaViewerDelegate::saveImageFileDialog); //保存图片

    //TODO(must): implement the openInFileExplorer functionality
    connect(view->openInFileExplorerAction,
            &QAction::triggered,
            this,
            &MediaViewerDelegate::openInFileExplorer);

    connect(view->rotateAction,
            &QAction::triggered,
            this,
            &MediaViewerDelegate::rotateImage); //旋转图片

    connect(view->deleteAction,
            &QAction::triggered,
            this,
            &MediaViewerDelegate::deleteImage); //删除图片

    connect(view->printAction, &QAction::triggered, this, [=]() {
        qDebug() << "Print action clicked";
    }); //打印操作

    connect(view->editAction, &QAction::triggered, this, [=]() {
        //TODO(optional): implement the edit functionality
    }); //编辑功能

    connect(view->prevAction,
            &QAction::triggered,
            this,
            &MediaViewerDelegate::prevImage); //上一张图片

    connect(view->nextAction,
            &QAction::triggered,
            this,
            &MediaViewerDelegate::nextImage); //下一张图片

    connect(view->likeButton, &ElaIconButton::clicked, this, [=, this]() {
        //TODO(must): implement the like functionality
        // add the image to Favorite Page
        bool currentState = mediaListModel
                                ->data(mediaListModel->index(mediaIndex.row(),
                                                             MediaListModel::IsFavorite))
                                .toBool();
        mediaListModel->setData(mediaListModel->index(mediaIndex.row(), MediaListModel::IsFavorite),
                                !currentState,
                                Qt::EditRole);
        updateLikeButtonState();
        if (!currentState) {
            ElaMessageBar::success(ElaMessageBarType::Bottom,
                                   "Added to favorites",
                                   nullptr,
                                   2000,
                                   view->imageViewer);
        } else {
            ElaMessageBar::error(ElaMessageBarType::Bottom,
                                 "Removed from favorites",
                                 nullptr,
                                 2000,
                                 view->imageViewer);
        }
    }); //点赞功能

    connect(view->fileInfoButton,
            &ElaIconButton::clicked,
            this,
            &MediaViewerDelegate::onFileInfoClicked); //查看文件信息

    connect(view->zoomInButton, &ElaIconButton::clicked, [this]() {
        scaleTo(getScale() + 10);
        view->zoomSlider->setValue(getScale());
    }); //放大图片

    connect(view->zoomSlider, &ElaSlider::valueChanged, [this](int value) {
        scaleTo(value);
    }); //缩放滑块的变化

    connect(view->zoomOutButton, &ElaIconButton::clicked, [this]() {
        scaleTo(view->imageViewer->getScale() - 10);
        view->zoomSlider->setValue(getScale());
    }); //缩小图片

    connect(view->maximizeButton, &ElaIconButton::clicked, [this]() {
        this->view->showMaximized();
    }); //最大化窗口

    connect(view->zoom2originalButton, &ElaIconButton::clicked, [this]() {
        scaleTo(100);
        view->zoomSlider->setValue(getScale());
    }); //原始大小恢复

    connect(this, &MediaViewerDelegate::scaledByWheel, [this]() {
        view->zoomSlider->setValue(getScale());
    }); //响应鼠标滚轮事件（缩放）

    connect(view->imageViewer,
            &ImageViewer::wheelScrolled,
            this,
            &MediaViewerDelegate::onWheelScrolled);
} //响应鼠标滚轮事件

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
        filepath = mediaIndex.data().value<QString>(); //获取对应行的文件路径
        loadImage(filepath);                           //加载新的图片文件并显示在视图中
    }
} //处理模型行移除函数

void MediaViewerDelegate::onImageChanged(bool fadeAnimation) {
    updateLikeButtonState();
    view->imageViewer->setContent(image, fadeAnimation); //是否使用淡入淡出动画
    view->fileInfoBriefText->setText(QString("%1 x %2 %3")
                                         .arg(QString::number(image.width()))
                                         .arg(QString::number(QImage(image).height()))
                                         .arg(Tools::fileSizeString(filepath))); //更新图像信息文本
    view->setWindowTitle(QFileInfo(filepath).fileName());                        //更新窗口标题
    view->zoomSlider->setValue(view->imageViewer->getScale());                   //更新缩放滑块的值
    view->fileInfoWidget->loadInfo(filepath); //加载并显示文件的详细信息
} //处理图像变化

void MediaViewerDelegate::onWheelScrolled(int delta) {
    if (settings.value("wheelBehavior").toInt() == 0) {
        const double scaleFactor = std::abs(delta) / 100.0;
        scaleTo(view->imageViewer->getScale() + delta / 10);
        emit scaledByWheel();
    } else {
        if (delta > 0) {
            prevImage();
        } else {
            nextImage();
        }
    } //根据 settings 中 wheelBehavior 的设置，判断是进行缩放操作还是切换图像操作
} //鼠标滚轮滚动

bool MediaViewerDelegate::copyImageToClipboard() {
    if (this->image.isNull()) {
        ElaMessageBar::error(ElaMessageBarType::Bottom,
                             "Null Image!",
                             nullptr,
                             2000,
                             view->imageViewer);
        return false; //检查图像是否为空
    } else {
        QClipboard* clipboard = QApplication::clipboard();
        clipboard->setImage(this->image); //复制到剪切板
        ElaMessageBar::success(ElaMessageBarType::Bottom,
                               "Copied!",
                               nullptr,
                               2000,
                               view->imageViewer);
        return true;
    }
} //复制图像到剪切板

void MediaViewerDelegate::openImageFileDialog() {
    filepath = QFileDialog::getOpenFileName(nullptr,
                                            "Choose Image File",
                                            "",
                                            "Image Files (*.png *.jpg *.bmp *.jpeg *.gif)");
    if (!filepath.isEmpty()) { //如果选择了文件
        loadImage(filepath);   //更新文件路径并调用函数加载该文件
    }
} //打开图像文件对话框函数

void MediaViewerDelegate::saveImageFileDialog() {
    filepath = QFileDialog::getSaveFileName(nullptr,
                                            "Save Image File",
                                            "",
                                            "Image Files (*.png *.jpg *.bmp *.jpeg *.gif)");
    if (!filepath.isEmpty()) {
        this->image.save(filepath);
    }
} //保存图像文件对话框函数

void MediaViewerDelegate::onFileInfoClicked() {
    auto* fileInfoAnimation = new QPropertyAnimation(view->fileInfoWidget,
                                                     "width"); //用于动画化fIw的宽度
    connect(fileInfoAnimation, &QPropertyAnimation::valueChanged, [this](const QVariant& value) {
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
} //处理文件信息点击函数

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
} //自适应调整媒体查看器窗口的大小

void MediaViewerDelegate::deleteImage() {
    if (settings.value("confirmDeletion").toBool()) { //读取设置判断用户是否启用删除确认对话框
        // ask before deletion
        auto* confirmDialog = new ElaContentDialog(view);  //设置了一个对话框
        confirmDialog->setWindowTitle("Confirm Deletion"); //标题
        auto* centralWidget = new QWidget(view);
        auto* centralVLayout = new QVBoxLayout(centralWidget); //垂直布局排布界面元素
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
            if (!QFile(filepath).remove()) { //如果尝试删除filepath失败
                ElaMessageBar::error(ElaMessageBarType::Bottom,
                                     "Delete failed!",
                                     nullptr,
                                     2000,
                                     view->imageViewer); //弹出错误消息
            } else {
                ElaMessageBar::success(ElaMessageBarType::Bottom,
                                       "Deleted!",
                                       nullptr,
                                       2000,
                                       view->imageViewer);
            }
        });
        confirmDialog->exec();
    } else { //直接进行删除操作
        if (!QFile(filepath).remove()) {
            ElaMessageBar::error(ElaMessageBarType::Bottom,
                                 "Delete failed!",
                                 nullptr,
                                 2000,
                                 view->imageViewer);
        } else {
            ElaMessageBar::success(ElaMessageBarType::Bottom,
                                   "Deleted!",
                                   nullptr,
                                   2000,
                                   view->imageViewer);
        }
    }
} //删除图像函数

void MediaViewerDelegate::prevImage() {
    if (mediaIndex.row() > 0) {
        mediaIndex = mediaListModel->index(mediaIndex.row() - 1, MediaListModel::Path);
    } else {
        mediaIndex = mediaListModel->index(mediaListModel->rowCount() - 1, MediaListModel::Path);
    }
    filepath = mediaIndex.data().value<QString>();
    loadImage(filepath);
} //切换到上一个图像

void MediaViewerDelegate::nextImage() {
    if (mediaIndex.row() < mediaListModel->rowCount() - 1) {
        mediaIndex = mediaListModel->index(mediaIndex.row() + 1, MediaListModel::Path);
    } else {
        mediaIndex = mediaListModel->index(0, MediaListModel::Path);
    }
    filepath = mediaIndex.data().value<QString>();
    loadImage(filepath);
} //下一个

void MediaViewerDelegate::rotateImage() {
    QTransform transform;
    transform.rotate(90);
    loadImage(image.transformed(transform), false);
    auto _ [[maybe_unused]] = QtConcurrent::run([this]() { this->image.save(filepath); });
} //旋转函数

bool MediaViewerDelegate::loadImage(const QString& path, bool fadeAnimation) {
    if (path.isEmpty()) {
        return false;
    } //检查图像是否为空
    try {
        QImage loaded(path);
        if (loaded.isNull()) {
            return false;
        } //若加载失败
        if (this->image.isNull() || this->image != loaded) {
            this->image = loaded;
            emit imageChanged(fadeAnimation);
            return true;
        }
        return false;
    } catch (...) {
    }
    return false;
} //加载图像

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
        return false;
    } catch (...) {
    }
    return false;
}

void MediaViewerDelegate::scaleTo(int percent) {
    view->imageViewer->scaleTo(percent);
} //将图像缩放到指定百分比

int MediaViewerDelegate::getScale() const {
    return view->imageViewer->getScale();
} //获取当前图像的缩放百分比

void MediaViewerDelegate::openInFileExplorer() {
    QProcess process;
    QString program;
    QStringList arguments;

#ifdef Q_OS_WIN
    program = "explorer";
    arguments << "/select," << filepath;
#elif defined(Q_OS_MAC)
    program = "open";
    arguments << "-R" << filepath;
#elif defined(Q_OS_LINUX)
    program = "xdg-open";
    arguments << QFileInfo(filepath).absolutePath();
#endif

    process.start(program, arguments);
    if (!process.waitForStarted()) {
        ElaMessageBar::error(ElaMessageBarType::Bottom,
                             "Failed to open file explorer",
                             nullptr,
                             2000,
                             view->imageViewer);
    }
}

void MediaViewerDelegate::setFilePath(const QString& path) {
    if (path != filepath) {
        filepath = path;
        int rowCount = mediaListModel->rowCount();
        for (int i = 0; i < rowCount; ++i) {
            QModelIndex currentIndex = mediaListModel->index(i, MediaListModel::Path);
            if (currentIndex.data().value<QString>() == path) {
                mediaIndex = currentIndex;
                break;
            }
        }
        loadImage(filepath);
    }
}

void MediaViewerDelegate::updateLikeButtonState() {
    if (view && view->likeButton) {
        bool isFavorite = mediaListModel
                              ->data(mediaListModel->index(mediaIndex.row(),
                                                           MediaListModel::IsFavorite))
                              .toBool();
        view->likeButton->setChecked(isFavorite);
    }
}
