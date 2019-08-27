#ifndef CELLIPSETOOL_H
#define CELLIPSETOOL_H
#include "idrawtool.h"

class CGraphicsEllipseItem;
class CEllipseTool : public IDrawTool
{
public:
    CEllipseTool();
    virtual ~CEllipseTool() Q_DECL_OVERRIDE;

public:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene) Q_DECL_OVERRIDE;
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene) Q_DECL_OVERRIDE;
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene)Q_DECL_OVERRIDE;

private:
    CGraphicsEllipseItem *m_pEllipseItem;
};

#endif // CELLIPSETOOL_H
