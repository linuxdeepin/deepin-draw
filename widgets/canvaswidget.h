#ifndef CANVASWIDGET_H
#define CANVASWIDGET_H

#include <QScrollArea>
#include <QLabel>
#include <QKeyEvent>
#include <QMouseEvent>

#include "canvaslabel.h"
#include "utils/baseutils.h"
#include "utils/shapesutils.h"

class CanvasWidget : public QScrollArea {
    Q_OBJECT
public:
    CanvasWidget(QWidget* parent = 0);
    ~CanvasWidget();

    void setImage(QString filename);

    bool overWindowSize();

    QSize fitWindowScaledSize(QSize windowSize, QSize imgSize);
    void zoomOutImage();
    void zoomInImage();

signals:
    void requestInitShapeWidget(QString shape, bool needInited);
    void changeShapeColor(DrawStatus drawstatus, QColor color);
    void changeShapeLineWidth(int linewidth);
    void changeLineShape(QString lineshape);

private:
    CanvasLabel* m_canvasLabel;
    QString m_currentFile;

    qreal m_scaleValue;
    QSize m_scaledSize;
};

#endif // CANVASWIDGET_H
