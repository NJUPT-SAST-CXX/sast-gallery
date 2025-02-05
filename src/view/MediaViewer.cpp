#include "MediaViewer.h"
#include <ElaMenu.h>
#include <ElaMenuBar.h>
#include <QDir>
#include <QScreen>
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

    // image view
    imageViewer = new ImageViewer(QPixmap::fromImage(delegate->getImage()), this);

    // file info widget
    fileInfoWidget = new FileInfoWidget(this);
    fileInfoWidget->loadInfo(delegate->getFilePath());
    fileInfoWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    fileInfoWidget->setFixedWidth(0);
    fileInfoWidget->setMessageBarParent(imageViewer);
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

    mainMiddleAreaLayout->addWidget(imageViewer);
    mainMiddleAreaLayout->addLayout(operationLayout);

    middleAreaLayout->addLayout(mainMiddleAreaLayout);
    middleAreaLayout->addWidget(fileInfoWidget);

    // Main Layout
    mainLayout->addWidget(menuBar);
    mainLayout->addLayout(middleAreaLayout);
    mainLayout->setAlignment(Qt::AlignTop);
}