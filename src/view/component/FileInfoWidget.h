#pragma once

#include <ElaIconButton.h>
#include <ElaText.h>
#include <QObject>
#include <QVBoxLayout>
#include <QWidget>

class FileInfoWidget : public QWidget {
    Q_OBJECT
public:
    explicit FileInfoWidget(QWidget* parent = nullptr);
    ~FileInfoWidget();

    void loadInfo(const QString& filepath);
    void setMessageBarParent(QWidget* parent);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    ElaText* nameText{};
    ElaText* resolutionText{};
    ElaText* sizeText{};
    ElaText* bitDepthText{};
    ElaText* createdText{};
    ElaText* lastModifiedText{};
    ElaText* pathText{};
    QWidget* messageBarParent{};

    static ElaIconButton* createIconDisplayer(ElaIconType::IconName awesome,
                                              QWidget* parent,
                                              int pixelSize = 16);
    QVBoxLayout* createContent(ElaIconType::IconName icon, QString label, QObject* content);
};