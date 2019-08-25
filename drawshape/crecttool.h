#ifndef CRECTTOOL_H
#define CRECTTOOL_H

#include "idrawtool.h"

class CGraphicsRectItem;
class CRectTool : public IDrawTool
{
public:
    CRectTool();
    virtual ~CRectTool();

public:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene);

private:
    CGraphicsRectItem *m_pRectItem;
};

#endif // CRECTTOOL_H
