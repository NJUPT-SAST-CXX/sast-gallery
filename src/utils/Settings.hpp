#pragma once

#include <QSettings>
#include <QSize>
#include <filesystem>

inline QSettings settings = [] {
    const std::filesystem::path configDir =
#ifdef Q_OS_WINDOWS
        std::filesystem::path(std::getenv("APPDATA")) / "Local" / "NJUPT-SAST-C++" / "SAST-Gallery";
#else
        std::filesystem::path(std::getenv("HOME")) / ".config" / "NJUPT-SAST-C++" / "SAST-Gallery";
#endif

    if (!std::filesystem::exists(configDir)) {
        std::filesystem::create_directories(configDir);
    }

    return QSettings(QString::fromStdString(configDir.string() + "/settings.ini"),
                     QSettings::IniFormat);
}();

inline void initSettings() {
    if (!settings.contains("windowSize")) {
        settings.setValue("windowSize", QSize(800, 600));
    }
    if (!settings.contains("theme")) {
        settings.setValue("theme", "light");
    }
    if (!settings.contains("micaEffect")) {
        settings.setValue("micaEffect", false);
    }
    if (!settings.contains("navigationBarDisplayMode")) {
        settings.setValue("navigationBarDisplayMode", 0);
    }
    if (!settings.contains("wheelBehavior")) {
        settings.setValue("wheelBehavior", 0);
    }
    if (!settings.contains("confirmDeletion")) {
        settings.setValue("confirmDeletion", true);
    }
}
