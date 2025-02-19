#include "MediaViewer.h"
#include <ElaMenu.h>
#include <ElaMenuBar.h>
#include <ElaToolTip.h>
#include <QFileInfo>
#include <delegate/MediaViewerDelegate.h>
#include <utils/Tools.h>

MediaViewer::MediaViewer(QAbstractItemModel* model, int index, QWidget* parent)
    : ElaWidget(parent)
    , delegate(new MediaViewerDelegate(model, index, this, this)) {
    initWindow();
    initContent();
    delegate->initConnections();
}

MediaViewer::~MediaViewer() {}

void MediaViewer::initWindow() {
    resize(1080, 720);                                                //窗口大小
    setMinimumSize(640, 480);                                         //最小大小
    QString fileName = QFileInfo(delegate->getFilePath()).fileName(); //获取文件名和路径
    setWindowTitle(fileName);
    setWindowButtonFlag(ElaAppBarType::ButtonType::StayTopButtonHint, false); //禁用置顶按钮
    setAttribute(Qt::WA_Hover);                                               //窗口悬停
}

void MediaViewer::initContent() {
    auto* mainLayout = new QVBoxLayout(this);       //垂直布局
    auto* middleAreaLayout = new QHBoxLayout(this); //水平布局
    auto* mainMiddleAreaLayout = new QVBoxLayout(this);

    // Create menu bar
    auto* menuBar = new ElaMenuBar(this); //菜单栏

    rotateAction = menuBar->addElaIconAction(ElaIconType::RotateRight, "rotate");
    deleteAction = menuBar->addElaIconAction(ElaIconType::TrashCan, "delete");
    printAction = menuBar->addElaIconAction(ElaIconType::Print, "print");
    editAction = menuBar->addElaIconAction(ElaIconType::Pen, "edit");
    prevAction = menuBar->addElaIconAction(ElaIconType::CircleChevronLeft, "previous");
    nextAction = menuBar->addElaIconAction(ElaIconType::CircleChevronRight, "next");

    ElaMenu* fileMenu = menuBar->addMenu(ElaIconType::Ellipsis, nullptr);
    openFileAction = fileMenu->addAction("Open");
    copyFileAction = fileMenu->addAction("Copy");
    saveFileAction = fileMenu->addAction("Save As");
    openInFileExplorerAction = fileMenu->addAction("Open image in File Explorer");

    fileMenu->addSeparator();

    //Set menu bar layout policy
    menuBar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    menuBar->setMinimumWidth(100);
    menuBar->setMaximumHeight(25);
    fileMenu->setMinimumWidth(50);

    // image view
    imageViewer = new ImageViewer(QPixmap::fromImage(delegate->getImage()), this); //图像查看器

    // file info widget
    fileInfoWidget = new FileInfoWidget(this);                                 //文件信息查看窗口
    fileInfoWidget->loadInfo(delegate->getFilePath());                         //加载文件信息
    fileInfoWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding); //窗口大小
    fileInfoWidget->setFixedWidth(0);                                          //固定宽度
    fileInfoWidget->setMessageBarParent(imageViewer);
    fileInfoWidget->hide();

    // Create buttons
    auto* operationLayout = new QHBoxLayout(this); //水平布局，放置操作按钮

    likeButton = new ElaIconButton(ElaIconType::Heart, this); //喜欢按钮
    likeButton->setMaximumWidth(25);

    fileInfoButton = new ElaIconButton(ElaIconType::CircleInfo);
    fileInfoButton->setMaximumWidth(25);

    auto* dividerText1 = new ElaText("|", this); //分隔文本
    dividerText1->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    dividerText1->setTextPixelSize(14);

    fileInfoBriefText //文件信息显示
        = new ElaText(QString("%1 x %2 %3")
                          .arg(QString::number(QImage(delegate->getFilePath()).width()))
                          .arg(QString::number(QImage(delegate->getFilePath()).height()))
                          .arg(Tools::fileSizeString(delegate->getFilePath())),
                      this);
    fileInfoBriefText->setWordWrap(false);
    fileInfoBriefText->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    fileInfoBriefText->setTextPixelSize(14);

    zoomInButton = new ElaIconButton(ElaIconType::MagnifyingGlassPlus);
    zoomInButton->setMaximumWidth(25);

    zoomSlider = new ElaSlider(Qt::Orientation::Horizontal); //控制缩放比例
    // range from 1% to 800%
    zoomSlider->setRange(1, 800);
    zoomSlider->setSingleStep(1);
    zoomSlider->setValue(100);
    zoomSlider->setMaximumWidth(300);

    zoomOutButton = new ElaIconButton(ElaIconType::MagnifyingGlassMinus);
    zoomOutButton->setMaximumWidth(25);

    ElaText* dividerText2 = new ElaText("|", this);
    dividerText2->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    dividerText2->setTextPixelSize(14);

    maximizeButton = new ElaIconButton(ElaIconType::ExpandWide);
    maximizeButton->setMaximumWidth(25);

    zoom2originalButton = new ElaIconButton(ElaIconType::Expand);
    zoom2originalButton->setMaximumWidth(25);

    operationLayout->addWidget(likeButton);
    operationLayout->addWidget(fileInfoButton);
    operationLayout->addWidget(dividerText1);
    operationLayout->addWidget(fileInfoBriefText);
    operationLayout->addStretch();
    operationLayout->addWidget(zoomOutButton);
    operationLayout->addWidget(zoomSlider);
    operationLayout->addWidget(zoomInButton);
    operationLayout->addWidget(dividerText2);
    operationLayout->addWidget(maximizeButton);
    operationLayout->addWidget(zoom2originalButton);

    mainMiddleAreaLayout->addWidget(imageViewer);
    mainMiddleAreaLayout->addLayout(operationLayout);

    middleAreaLayout->addLayout(mainMiddleAreaLayout);
    middleAreaLayout->addWidget(fileInfoWidget);

    // Main Layout
    mainLayout->addWidget(menuBar);
    mainLayout->addLayout(middleAreaLayout);
    mainLayout->setAlignment(Qt::AlignTop);

    // tooltips
    auto* likeButtonToolTip = new ElaToolTip(likeButton);
    likeButtonToolTip->setToolTip("Like");
    auto* fileInfoButtonToolTip = new ElaToolTip(fileInfoButton);
    fileInfoButtonToolTip->setToolTip("File Info");
    auto* zoomInButtonToolTip = new ElaToolTip(zoomInButton);
    zoomInButtonToolTip->setToolTip("Zoom In");
    auto* zoomOutButtonToolTip = new ElaToolTip(zoomOutButton);
    zoomOutButtonToolTip->setToolTip("Zoom Out");
    auto* maximizeButtonToolTip = new ElaToolTip(maximizeButton);
    maximizeButtonToolTip->setToolTip("Maximize");
    auto* zoom2originalButtonToolTip = new ElaToolTip(zoom2originalButton);
    zoom2originalButtonToolTip->setToolTip("Zoom to Original");
    auto* zoomSliderToolTip = new ElaToolTip(zoomSlider);
    zoomSliderToolTip->setToolTip("100%");
    connect(zoomSlider, &QSlider::valueChanged, this, [=](int value) {
        zoomSliderToolTip->setToolTip(QString("%1%").arg(value));
    });
}

void MediaViewer::setFilePath(const QString& path) {
    delegate->setFilePath(path);
    QString fileName = QFileInfo(path).fileName();
    setWindowTitle(fileName);
    fileInfoBriefText->setText(QString("%1 x %2 %3")
                                   .arg(QString::number(QImage(path).width()))
                                   .arg(QString::number(QImage(path).height()))
                                   .arg(Tools::fileSizeString(path)));
    fileInfoWidget->loadInfo(path);
}
