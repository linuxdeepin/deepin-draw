#ifndef CPOLYGONALSTARTOOL_H
#define CPOLYGONALSTARTOOL_H
#include "idrawtool.h"

class CGraphicsPolygonalStarItem;

class CPolygonalStarTool : public IDrawTool
{
public:
    CPolygonalStarTool();

    virtual ~CPolygonalStarTool() Q_DECL_OVERRIDE;

public:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene) Q_DECL_OVERRIDE;
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene) Q_DECL_OVERRIDE;
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene)Q_DECL_OVERRIDE;

private:
    CGraphicsPolygonalStarItem *m_pPolygonalStarItem;
};

#endif // CPOLYGONALSTARTOOL_H
