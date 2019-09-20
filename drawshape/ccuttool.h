#ifndef CCUTTOOL_H
#define CCUTTOOL_H

#include "idrawtool.h"
#include "cgraphicscutitem.h"


class CCutTool : public IDrawTool
{
public:
    CCutTool();
    virtual ~CCutTool() Q_DECL_OVERRIDE;

public:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene) Q_DECL_OVERRIDE;
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene) Q_DECL_OVERRIDE;
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene)Q_DECL_OVERRIDE;

private:
    CGraphicsCutItem *m_pCutItem;
};

#endif // CCUTTOOL_H
