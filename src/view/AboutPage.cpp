#include "AboutPage.h"
#include <ElaIconButton.h>
#include <ElaImageCard.h>
#include <ElaMessageBar.h>
#include <QApplication>
#include <QClipboard>
#include <QVBoxLayout>
#include <Version.h>

AboutPage::AboutPage(QWidget* parent)
    : BasePage(parent) {
    setWindowTitle("About");

    auto* pixCard = new ElaImageCard(this);
    pixCard->setFixedSize(100, 100);
    pixCard->setIsPreserveAspectCrop(false);
    pixCard->setCardImage(QImage(":/res/icon/app_icon.svg"));

    auto* pixCardText = new ElaText("SAST Gallery", this);
    pixCardText->setWordWrap(false);
    pixCardText->setTextPixelSize(25);

    auto* pixCardLayout = new QHBoxLayout();
    pixCardLayout->addWidget(pixCard);
    pixCardLayout->addSpacing(20);
    pixCardLayout->addWidget(pixCardText);
    pixCardLayout->setAlignment(Qt::AlignCenter);

    auto* centralWidget = new QWidget(this);
    centralWidget->setWindowTitle("About");
    centralWidget->setSizePolicy(QSizePolicy::Policy::Ignored,
                                 centralWidget->sizePolicy().verticalPolicy());
    auto centerLayout = new QVBoxLayout(centralWidget);
    centerLayout->addSpacing(30);
    centerLayout->addLayout(pixCardLayout);
    centerLayout->addSpacing(30);
    centerLayout->addWidget(createTextArea("Version Info", VERSION_FULL));
    centerLayout->addWidget(createTextArea("License", "MIT License Copyright (c) 2024 NJUPT-SAST"));
    centerLayout->addWidget(createTextArea("Supported Platforms", "Windows | Linux | macOS"));
    centerLayout->addWidget(
        createLinkArea("Repository", "https://github.com/NJUPT-SAST-CXX/sast-gallery"));
    centerLayout->addWidget(createLinkArea("Visit SAST", "https://sast.fun"));
    centerLayout->addStretch();
    centerLayout->setContentsMargins(0, 0, 20, 0);
    addCentralWidget(centralWidget, true, true, 0);
}

ElaScrollPageArea* AboutPage::createTextArea(QString label, QString content) {
    auto* contentText = new ElaText(content, this);
    contentText->setWordWrap(false);
    contentText->setTextPixelSize(15);
    return createScrollPageArea(label, contentText);
}

ElaScrollPageArea* AboutPage::createLinkArea(QString label, QString link) {
    auto* copyButton = new ElaIconButton(ElaIconType::Copy, 15, 15, 15, this);
    auto* linkText = new ElaText(QString("<a style='color: #D0BCFF' href = \"%1\">%1").arg(link),
                                 this);
    linkText->setWordWrap(false);
    linkText->setTextPixelSize(15);
    linkText->setOpenExternalLinks(true);
    connect(copyButton, &QPushButton::clicked, [=]() {
        QApplication::clipboard()->setText(link);
        ElaMessageBar::success(ElaMessageBarType::BottomRight, "Copied!", nullptr, 2000);
    });
    return createScrollPageArea(label, copyButton, linkText);
}
