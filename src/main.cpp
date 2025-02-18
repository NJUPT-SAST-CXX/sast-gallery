#include "MainWindow.h"
#include <ElaApplication.h>
#include <ElaTheme.h>
#include <QApplication>
#include <QSize>
#include <utils/Settings.hpp> //管理应用程序的设置

int main(int argc, char* argv[]) {
    QApplication a(argc, argv); //核心对象，负责处理命令行参数、管理应用程序的资源和循环
    QApplication::setWindowIcon(QIcon(":/res/icon/app_icon.svg")); //设置窗口图标
    initSettings();                                                //初始化程序设置
    ElaApplication::getInstance()->init();                         //单例实例，初始化环境
    ElaTheme::getInstance()->setThemeMode(                         //获取单例对象，并设置主题
        settings.value("theme").toString() == "light" ? ElaThemeType::Light : ElaThemeType::Dark);
    MainWindow w;
    w.show();
    return a.exec(); //进入事件循环
}
