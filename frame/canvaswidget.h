#ifndef CANVASWIDGET_H
#define CANVASWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QKeyEvent>
#include <QMouseEvent>

#include "utils/baseutils.h"
#include "utils/shapesutils.h"
#include "drawshape/shapeswidget.h"

class CanvasWidget : public QLabel
{
    Q_OBJECT

public:
    CanvasWidget(QWidget* parent = 0);
    ~CanvasWidget();

signals:
    void drawShapeChanged(QString shape);
    void fillShapeSelectedActive(bool selected);
    void rotateImage(int degree);
    void mirroredImage(bool horizontal, bool vertical);
    void saveImage(const QString &path);

    void updateMiddleWidget(QString type);

private:
    QString m_currentFile;
    ShapesWidget* m_shapesWidget;

    qreal m_scaleValue;
    QSize m_scaledSize;
};

#endif // CANVASWIDGET_H
