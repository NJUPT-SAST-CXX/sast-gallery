#pragma once

#include <QGraphicsView>
#include <QMouseEvent>

class AbstractMediaViewer : public QGraphicsView {
    Q_OBJECT

public:
    explicit AbstractMediaViewer(QWidget* parent = nullptr);
    virtual ~AbstractMediaViewer() = default;

    [[nodiscard]] int getScale() const;
    [[nodiscard]] int getMinScale() const;
    [[nodiscard]] int getMaxScale() const;
    void setMinScale(int scale);
    void setMaxScale(int scale);
    void scaleTo(int scale);

signals:
    void wheelScrolled(int delta);
    void resized();

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;

    QGraphicsScene* scene;
    QPoint lastMousePos;
    bool dragging;
    int cntScale;
    int minScale;
    int maxScale;

    virtual void adjustContentToFit() = 0;
}; 