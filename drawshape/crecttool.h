#ifndef CRECTTOOL_H
#define CRECTTOOL_H

#include "idrawtool.h"
#include "cgraphicsrectitem.h"
class CRectTool : public IDrawTool
{
public:
    CRectTool();
    virtual ~CRectTool() Q_DECL_OVERRIDE;

public:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene) Q_DECL_OVERRIDE;
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene) Q_DECL_OVERRIDE;
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene)Q_DECL_OVERRIDE;

private:
    CGraphicsRectItem *m_pRectItem;
};

#endif // CRECTTOOL_H
