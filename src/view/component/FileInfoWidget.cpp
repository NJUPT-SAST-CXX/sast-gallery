#include "FileInfoWidget.h"
#include "utils/Tools.h"
#include <ElaIcon.h>
#include <ElaMessageBar.h>
#include <ElaTheme.h>
#include <QApplication>
#include <QClipboard>
#include <QFileInfo>
#include <QHBoxLayout>
#include <QImageReader>
#include <QLabel>
#include <QPainter>
#include <QString>
#include <QVBoxLayout>

FileInfoWidget::FileInfoWidget(QWidget* parent)
    : QWidget(parent) {
    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);

    auto* titleText = new ElaText("File Info", 25, this);
    auto* titleIcon = createIconDisplayer(ElaIconType::CircleInfo, this, 20);
    titleIcon->setFixedSize(25, 25);
    auto* titleLayout = new QHBoxLayout(this);
    titleLayout->addWidget(titleIcon);
    titleLayout->addWidget(titleText);

    nameText = new ElaText("helloworld.png", 13, this);

    auto* sizeInfoContentLayout = new QHBoxLayout(this);
    resolutionText = new ElaText("1080 x 720", 13, this);
    sizeText = new ElaText("1.5 MB", 13, this);
    bitDepthText = new ElaText("24 bit", 13, this);
    sizeInfoContentLayout->addWidget(resolutionText);
    sizeInfoContentLayout->addSpacing(10);
    sizeInfoContentLayout->addWidget(sizeText);
    sizeInfoContentLayout->addSpacing(10);
    sizeInfoContentLayout->addWidget(bitDepthText);
    sizeInfoContentLayout->addStretch();

    createdText = new ElaText("1887.10.31", 13, this);

    lastModifiedText = new ElaText("1975.04.05", 13, this);

    auto* copyButton = new ElaIconButton(ElaIconType::Copy, 15, 15, 15, this);
    pathText = new ElaText("C:/User/SAST/Pictures/helloworld.png", 13, this);
    auto* filePathLayout = new QHBoxLayout(this);
    filePathLayout->addWidget(pathText);
    filePathLayout->addWidget(copyButton);
    connect(copyButton, &ElaIconButton::clicked, [this] {
        QApplication::clipboard()->setText(pathText->text());
        ElaMessageBar::success(ElaMessageBarType::BottomRight, "Copied!", nullptr, 2000);
    });

    mainLayout->addLayout(titleLayout);
    mainLayout->addSpacing(25);
    mainLayout->addLayout(createContent(ElaIconType::Image, "name", nameText));
    mainLayout->addSpacing(10);
    mainLayout->addLayout(createContent(ElaIconType::Cube, "size", sizeInfoContentLayout));
    mainLayout->addSpacing(10);
    mainLayout->addLayout(createContent(ElaIconType::Calendar, "created", createdText));
    mainLayout->addSpacing(10);
    mainLayout->addLayout(
        createContent(ElaIconType::CalendarLinesPen, "modified", lastModifiedText));
    mainLayout->addSpacing(10);
    mainLayout->addLayout(createContent(ElaIconType::FolderOpen, "path", filePathLayout));
    mainLayout->addStretch();
    setLayout(mainLayout);
}

FileInfoWidget::~FileInfoWidget() {}

void FileInfoWidget::loadInfo(const QString& filepath) {
    QFileInfo fileInfo(filepath);
    nameText->setText(fileInfo.fileName());
    resolutionText->setText(
        QString("%1 x %2").arg(QImage(filepath).width()).arg(QImage(filepath).height()));
    sizeText->setText(Tools::fileSizeString(filepath));
    bitDepthText->setText(QString("%1 bit").arg(Tools::imageBitDepth(QImage(filepath))));

    QString dateTimeFormat = "yyyy.MM.dd   HH:mm   ddd";
    createdText->setText(fileInfo.birthTime().toString(dateTimeFormat));
    lastModifiedText->setText(fileInfo.lastModified().toString(dateTimeFormat));

    pathText->setText(fileInfo.absoluteFilePath());
}

void FileInfoWidget::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    painter.save();
    painter.setPen(ElaThemeColor(ElaTheme::getInstance()->getThemeMode(), BasicBorder));
    painter.setBrush(ElaThemeColor(ElaTheme::getInstance()->getThemeMode(), BasicBaseAlpha));
    QRect foregroundRect(1, 1, width() - 2, height() - 2);
    painter.drawRoundedRect(foregroundRect, 6, 6);
    painter.restore();
}

ElaIconButton* FileInfoWidget::createIconDisplayer(ElaIconType::IconName awesome,
                                                   QWidget* parent,
                                                   int pixelSize) {
    auto* iconDisplayer = new ElaIconButton(awesome, pixelSize, parent);
    iconDisplayer->setFixedSize(20, 20);
    iconDisplayer->setLightHoverColor(QColor(0, 0, 0, 0));
    iconDisplayer->setDarkHoverColor(QColor(0, 0, 0, 0));
    return iconDisplayer;
}

QVBoxLayout* FileInfoWidget::createContent(ElaIconType::IconName icon,
                                           QString label,
                                           QObject* content) {
    auto* mainLayout = new QVBoxLayout(this);

    auto* labelLayout = new QHBoxLayout(this);
    auto* contentIcon = createIconDisplayer(icon, this);
    auto* contentLabel = new ElaText(label, this);
    contentLabel->setTextPixelSize(16);
    labelLayout->addWidget(contentIcon);
    labelLayout->addWidget(contentLabel);

    auto* contentLayout = new QHBoxLayout(this);
    auto* placeHolder = new QLabel(this);
    placeHolder->setFixedSize(contentIcon->size());
    contentLayout->addWidget(placeHolder);
    if (auto* widget = qobject_cast<QWidget*>(content)) {
        contentLayout->addWidget(widget);
    } else if (auto* layout = qobject_cast<QLayout*>(content)) {
        contentLayout->addLayout(layout);
    } else {
        qDebug() << "content is not a QWidget or QLayout!";
        return nullptr;
    }

    mainLayout->addLayout(labelLayout);
    mainLayout->addLayout(contentLayout);

    return mainLayout;
}