#ifndef CPENTOOL_H
#define CPENTOOL_H
#include "idrawtool.h"
#include "cgraphicspenitem.h"

class CPenTool : public IDrawTool
{
public:
    CPenTool();
    virtual ~CPenTool() Q_DECL_OVERRIDE;

public:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene) Q_DECL_OVERRIDE;
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene) Q_DECL_OVERRIDE;
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene)Q_DECL_OVERRIDE;

private:
    CGraphicsPenItem *m_pPenItem;
};

#endif // CPENTOOL_H
