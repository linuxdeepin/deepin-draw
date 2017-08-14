#ifndef CANVASWIDGET_H
#define CANVASWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QKeyEvent>
#include <QMouseEvent>

#include "canvaslabel.h"
#include "utils/baseutils.h"
#include "utils/shapesutils.h"
#include "scen/imageview.h"

class CanvasWidget : public QWidget {
    Q_OBJECT
public:
    CanvasWidget(QWidget* parent = 0);
    ~CanvasWidget();

    void setImage(QString filename);

    bool overWindowSize();

    QSize fitWindowScaledSize(QSize windowSize, QSize imgSize);

signals:
    void requestInitShapeWidget(QString shape);
    void changeShapeColor(DrawStatus drawstatus, QColor color);
    void changeShapeLineWidth(int linewidth);
    void changeLineShape(QString lineshape);
    void changeTextFontsize(int fontsize);
    void changedBlurLinewidth(int linewidth);

private:
//    CanvasLabel* m_canvasLabel;
    ImageView* m_view;
    QString m_currentFile;

    qreal m_scaleValue;
    QSize m_scaledSize;
};

#endif // CANVASWIDGET_H
