#include "MainWindow.h"
#include <ElaApplication.h>
#include <ElaTheme.h>
#include <QApplication>
#include <QSize>
#include <utils/Settings.hpp>

int main(int argc, char* argv[]){
    QApplication a(argc, argv);
    QApplication::setWindowIcon(QIcon(":/res/icon/app_icon.svg"));
    initSettings();
    ElaApplication::getInstance()->init();
    ElaTheme::getInstance()->setThemeMode(
        settings.value("theme").toString() == "light" ? ElaThemeType::Light : ElaThemeType::Dark);
    MainWindow w;
    w.show();
    return a.exec();
}
