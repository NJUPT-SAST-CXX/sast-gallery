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
    auto* window = dynamic_cast<ElaWindow*>(parent); //动态类型转换，确保其实际指向一个ElaWindow*对象
    setWindowTitle("Setting");

    auto appearanceText = new ElaText("Appearance", this);
    appearanceText->setWordWrap(false);
    appearanceText->setTextPixelSize(18); //外观标题

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
    }); //更新themeComboBox的选中项，，确保不触发不必要的信息回调
    themeComboBox->setCurrentIndex(settings.value("theme").toString()
                                   == "dark"); //根据保存的主题设置初始化

    auto micaSwitchButton = new ElaToggleSwitch(this); //开关
    auto micaSwitchArea = createScrollPageArea("Mica Effect", micaSwitchButton);
    connect(micaSwitchButton, &ElaToggleSwitch::toggled, this, [=](bool checked) {
        eApp->setIsEnableMica(checked);           //设置启用状态
        settings.setValue("micaEffect", checked); //保存
    });
    micaSwitchButton->setIsToggled(
        settings.value("micaEffect").toBool()); //根据之前保存的云母效果设置初始化开关状态。

    auto minimumButton = new ElaRadioButton("Minimum", this);
    auto compactButton = new ElaRadioButton("Compact", this);
    auto maximumButton = new ElaRadioButton("Maximum", this);
    auto autoButton = new ElaRadioButton("Auto", this); //创建导航栏四种模式
    auto displayModeArea = createScrollPageArea("Navigation Bar Display Mode",
                                                minimumButton,
                                                compactButton,
                                                maximumButton,
                                                autoButton); //创建滚动页面区域
    connect(minimumButton, &ElaRadioButton::toggled, this, [=](bool checked) {
        if (checked) {
            window->setNavigationBarDisplayMode(ElaNavigationType::Minimal); //设置
            settings.setValue("navigationBarDisplayMode", 1);                //保存
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
    } //设置初始化选中状态

    auto* functionsText = new ElaText("Functions", this);
    functionsText->setWordWrap(false);
    functionsText->setTextPixelSize(18);

    auto wheelComboBox = new ElaComboBox(this);
    wheelComboBox->addItem("Zoom");
    wheelComboBox->addItem("Switch");
    auto wheelSwitchArea = createScrollPageArea("Wheel Behaviors", wheelComboBox);
    // TODO: implement the rest

    int wheelBehavior = settings.value("wheelBehavior", 0).toInt();
    wheelComboBox->setCurrentIndex(wheelBehavior);
    connect(wheelComboBox,
            QOverload<int>::of(&ElaComboBox::currentIndexChanged),
            this,
            [=](int index) { settings.setValue("wheelBehavior", index); });

    auto deletionSwitchButton = new ElaToggleSwitch(this);
    auto deletionSwitchArea = createScrollPageArea("Ask for deletion permission",
                                                   deletionSwitchButton);
    // TODO: implement the rest

    bool confirmDeletion = settings.value("confirmDeletion", true).toBool();
    deletionSwitchButton->setIsToggled(confirmDeletion);
    connect(deletionSwitchButton, &ElaToggleSwitch::toggled, this, [=](bool checked) {
        settings.setValue("confirmDeletion", checked);
    });

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