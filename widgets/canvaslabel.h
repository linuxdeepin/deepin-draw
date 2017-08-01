#ifndef CANVASLABEL_H
#define CANVASLABEL_H

#include <QLabel>
#include <QPaintEvent>

#include "drawshape/shapeswidget.h"
#include "utils/baseutils.h"

class CanvasLabel : public QLabel {
    Q_OBJECT
public:
    CanvasLabel(QWidget* parent = 0);
    ~CanvasLabel();

    void setCanvasPixmap(QString imageFile);
    void setCanvasPixmap(QPixmap pixmap);
    void initShapesWidget(QString shape, bool needInited);
    void setShapeColor(DrawStatus drawstatus, QColor color);

protected:
    void paintEvent(QPaintEvent* e);

private:
    bool m_shapesWidgetExist;
    QString m_currentFile;
    QPixmap m_currentPixmap;

    ShapesWidget* m_shapesWidget;
};

#endif // CANVASLABEL_H
