#include "MediaViewer.h"
#include "component/VideoViewer.h"
#include <ElaMenu.h>
#include <ElaMenuBar.h>
#include <ElaToolTip.h>
#include <QFileInfo>
#include <delegate/MediaViewerDelegate.h>
#include <model/Media.h>
#include <utils/Tools.h>

MediaViewer::MediaViewer(QAbstractItemModel* model, int index, QWidget* parent)
    : ElaWidget(parent)
    , delegate(new MediaViewerDelegate(model, index, this, this)) {
    initWindow();
    initContent();
    delegate->init();
    delegate->initConnections();
}

MediaViewer::~MediaViewer() {}

void MediaViewer::initWindow() {
    resize(1080, 720);
    setMinimumSize(640, 480);
    QString fileName = QFileInfo(delegate->getFilePath()).fileName();
    setWindowTitle(fileName);
    setWindowButtonFlag(ElaAppBarType::ButtonType::StayTopButtonHint, false);
    setAttribute(Qt::WA_Hover);
}

void MediaViewer::initContent() {
    auto* mainLayout = new QVBoxLayout(this);
    auto* middleAreaLayout = new QHBoxLayout(this);
    auto* mainMiddleAreaLayout = new QVBoxLayout(this);

    // Create menu bar
    auto* menuBar = new ElaMenuBar(this);

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

    // media viewer
    QString filePath = delegate->getFilePath();
    Media media(filePath);

    if (media.type == MediaType::Video) {
        abstractMediaViewer = new VideoViewer(filePath, this);
    } else if (media.type == MediaType::Image) {
        abstractMediaViewer = new ImageViewer(filePath, this);
    }

    // file info widget
    fileInfoWidget = new FileInfoWidget(this);
    fileInfoWidget->loadInfo(delegate->getFilePath());
    fileInfoWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    fileInfoWidget->setFixedWidth(0);
    fileInfoWidget->setMessageBarParent(abstractMediaViewer);
    fileInfoWidget->hide();

    // Create buttons
    QHBoxLayout* operationLayout = new QHBoxLayout(this);

    likeButton = new ElaIconButton(ElaIconType::Heart, this);
    likeButton->setMaximumWidth(25);

    fileInfoButton = new ElaIconButton(ElaIconType::CircleInfo);
    fileInfoButton->setMaximumWidth(25);

    ElaText* dividerText1 = new ElaText("|", this);
    dividerText1->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    dividerText1->setTextPixelSize(14);

    fileInfoBriefText
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

    zoomSlider = new ElaSlider(Qt::Orientation::Horizontal);
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

    mainMiddleAreaLayout->addWidget(abstractMediaViewer);
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

    // Disable actions and buttons for specific media types
    if (media.type == MediaType::Video) {
        rotateAction->setEnabled(false);
        printAction->setEnabled(false);
        zoomInButton->setEnabled(false);
        zoomOutButton->setEnabled(false);
        zoomSlider->setEnabled(false);
    } else if (media.type == MediaType::Image) {
    }
}