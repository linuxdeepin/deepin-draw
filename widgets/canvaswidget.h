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
    void initShapeWidget(QString shape);
    void shapeColorChanged(DrawStatus drawstatus, QColor color);
    void shapeLineWidthChanged(int linewidth);
    void lineShapeChanged(QString lineshape);
    void textFontsizeChanged(int fontsize);
    void blurLinewidthChanged(int linewidth);
    void rotateImage(const QString &path, int degree);
    void mirroredImage(bool horizontal, bool vertical);
    void cutImage();

private:
//    CanvasLabel* m_canvasLabel;
    ImageView* m_view;
    QString m_currentFile;

    qreal m_scaleValue;
    QSize m_scaledSize;
};

#endif // CANVASWIDGET_H
