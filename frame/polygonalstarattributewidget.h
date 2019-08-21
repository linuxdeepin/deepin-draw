#ifndef POLYGONALSTARATTRIBUTEWIDGET_H
#define POLYGONALSTARATTRIBUTEWIDGET_H

#include <DWidget>
#include "utils/baseutils.h"

DWIDGET_USE_NAMESPACE

class PolygonalStarAttributeWidget : public DWidget
{
    Q_OBJECT
public:
    PolygonalStarAttributeWidget(QWidget *parent = nullptr);
    ~PolygonalStarAttributeWidget();

signals:
    void resetColorBtns();
    void showColorPanel(DrawStatus drawstatus,
                        QPoint pos, bool visible = true);
};

#endif // POLYGONALSTARATTRIBUTEWIDGET_H
