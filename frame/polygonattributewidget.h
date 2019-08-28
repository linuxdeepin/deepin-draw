#ifndef POLYGONATTRIBUTEWIDGET_H
#define POLYGONATTRIBUTEWIDGET_H

#include <DWidget>

#include "drawshape/globaldefine.h"

DWIDGET_USE_NAMESPACE

class PolygonAttributeWidget : public DWidget
{
    Q_OBJECT
public:
    PolygonAttributeWidget(QWidget *parent = nullptr);
    ~PolygonAttributeWidget();

signals:
    void resetColorBtns();
    void showColorPanel(DrawStatus drawstatus,
                        QPoint pos, bool visible = true);
};


#endif // POLYGONATTRIBUTEWIDGET_H
