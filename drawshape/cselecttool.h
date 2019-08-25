#ifndef CSELECTTOOL_H
#define CSELECTTOOL_H
#include "idrawtool.h"

class CSelectTool : public IDrawTool
{
public:
    CSelectTool();
    virtual ~CSelectTool();
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene);
};

#endif // CSELECTTOOL_H
