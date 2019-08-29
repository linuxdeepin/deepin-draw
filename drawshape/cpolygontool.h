#ifndef CPOLYGONTOOL_H
#define CPOLYGONTOOL_H

#include "idrawtool.h"
#include "cgraphicspolygonitem.h"

class CPolygonTool : public IDrawTool
{
public:
    CPolygonTool();
    virtual ~CPolygonTool() Q_DECL_OVERRIDE;

public:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene) Q_DECL_OVERRIDE;
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene) Q_DECL_OVERRIDE;
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene)Q_DECL_OVERRIDE;

private:
    CGraphicsPolygonItem *m_pPolygonItem;
};

#endif // CPOLYGONTOOL_H
