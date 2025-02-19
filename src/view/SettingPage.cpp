#include "SettingPage.h"
#include "utils/Settings.hpp"
#include <ElaApplication.h>
#include <ElaComboBox.h>
#include <ElaRadioButton.h>
#include <ElaTheme.h>
#include <ElaToggleSwitch.h>
#include <ElaWindow.h>
#include <QVBoxLayout>

SettingPage::SettingPage(QWidget* parent)
    : BasePage(parent) {
    auto* window = dynamic_cast<ElaWindow*>(parent);
    setWindowTitle("Setting");

    auto appearanceText = new ElaText("Appearance", this);
    appearanceText->setWordWrap(false);
    appearanceText->setTextPixelSize(18);

    auto themeComboBox = new ElaComboBox(this);
    themeComboBox->addItem("Light");
    themeComboBox->addItem("Dark");
    auto themeSwitchArea = createScrollPageArea("Themes", themeComboBox);
    connect(themeComboBox,
            QOverload<int>::of(&ElaComboBox::currentIndexChanged),
            this,
            [=](int index) {
                if (index == 0) {
                    eTheme->setThemeMode(ElaThemeType::Light);
                    settings.setValue("theme", "light");
                } else {
                    eTheme->setThemeMode(ElaThemeType::Dark);
                    settings.setValue("theme", "dark");
                }
            });
    connect(eTheme, &ElaTheme::themeModeChanged, this, [=, this](ElaThemeType::ThemeMode themeMode) {
        themeComboBox->blockSignals(true);
        themeComboBox->setCurrentIndex(themeMode);
        themeComboBox->blockSignals(false);
    });
    themeComboBox->setCurrentIndex(settings.value("theme").toString() == "dark");

    auto micaSwitchButton = new ElaToggleSwitch(this);
    auto micaSwitchArea = createScrollPageArea("Mica Effect", micaSwitchButton);
    connect(micaSwitchButton, &ElaToggleSwitch::toggled, this, [=](bool checked) {
        eApp->setIsEnableMica(checked);
        settings.setValue("micaEffect", checked);
    });
    micaSwitchButton->setIsToggled(settings.value("micaEffect").toBool());

    auto minimumButton = new ElaRadioButton("Minimum", this);
    auto compactButton = new ElaRadioButton("Compact", this);
    auto maximumButton = new ElaRadioButton("Maximum", this);
    auto autoButton = new ElaRadioButton("Auto", this);
    auto displayModeArea = createScrollPageArea("Navigation Bar Display Mode",
                                                minimumButton,
                                                compactButton,
                                                maximumButton,
                                                autoButton);
    connect(minimumButton, &ElaRadioButton::toggled, this, [=](bool checked) {
        if (checked) {
            window->setNavigationBarDisplayMode(ElaNavigationType::Minimal);
            settings.setValue("navigationBarDisplayMode", 1);
        }
    });
    connect(compactButton, &ElaRadioButton::toggled, this, [=](bool checked) {
        if (checked) {
            window->setNavigationBarDisplayMode(ElaNavigationType::Compact);
            settings.setValue("navigationBarDisplayMode", 2);
        }
    });
    connect(maximumButton, &ElaRadioButton::toggled, this, [=](bool checked) {
        if (checked) {
            window->setNavigationBarDisplayMode(ElaNavigationType::Maximal);
            settings.setValue("navigationBarDisplayMode", 3);
        }
    });
    connect(autoButton, &ElaRadioButton::toggled, this, [=](bool checked) {
        if (checked) {
            window->setNavigationBarDisplayMode(ElaNavigationType::Auto);
            settings.setValue("navigationBarDisplayMode", 0);
        }
    });
    if (settings.value("navigationBarDisplayMode").toInt() == 1) {
        minimumButton->setChecked(true);
    } else if (settings.value("navigationBarDisplayMode").toInt() == 2) {
        compactButton->setChecked(true);
    } else if (settings.value("navigationBarDisplayMode").toInt() == 3) {
        maximumButton->setChecked(true);
    } else {
        autoButton->setChecked(true);
    }

    auto* functionsText = new ElaText("Functions", this);
    functionsText->setWordWrap(false);
    functionsText->setTextPixelSize(18);

    auto wheelComboBox = new ElaComboBox(this);
    wheelComboBox->addItem("Zoom");
    wheelComboBox->addItem("Switch");
    auto wheelSwitchArea = createScrollPageArea("Wheel Behaviors", wheelComboBox);
    connect(wheelComboBox,
            QOverload<int>::of(&ElaComboBox::currentIndexChanged),
            this,
            [=](int index) {
                                    settings.setValue("wheelBehavior",index);
//                if (index == 0) {
//                }
//                    settings.setValue("wheelBehavior",0);
//                } else {
//                    settings.setValue("wheelBehavior",1);
//                }
            });
    // TODO: implement the rest

    auto deletionSwitchButton = new ElaToggleSwitch(this);
    auto deletionSwitchArea = createScrollPageArea("Ask for deletion permission",
                                                   deletionSwitchButton);
    connect(deletionSwitchButton,&ElaToggleSwitch::toggled,this,[=](bool checked)
            {
        settings.setValue("confirmDeletion",checked);
            });
    // TODO: implement the rest

    auto centralWidget = new QWidget(this);
    centralWidget->setWindowTitle("Setting");
    auto centerLayout = new QVBoxLayout(centralWidget);
    centerLayout->addSpacing(30);
    centerLayout->addWidget(appearanceText);
    centerLayout->addSpacing(10);
    centerLayout->addWidget(themeSwitchArea);
    centerLayout->addWidget(micaSwitchArea);
    centerLayout->addWidget(displayModeArea);
    centerLayout->addSpacing(15);
    centerLayout->addWidget(functionsText);
    centerLayout->addSpacing(10);
    centerLayout->addWidget(wheelSwitchArea);
    centerLayout->addWidget(deletionSwitchArea);
    centerLayout->addStretch();
    centerLayout->setContentsMargins(0, 0, 20, 0);
    addCentralWidget(centralWidget, true, true, 0);
}
