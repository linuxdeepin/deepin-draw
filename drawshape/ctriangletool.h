#ifndef CTRIANGLETOOL_H
#define CTRIANGLETOOL_H
#include "idrawtool.h"

class CGraphicsTriangleItem;

class CTriangleTool : public IDrawTool
{
public:
    CTriangleTool();

    virtual ~CTriangleTool() Q_DECL_OVERRIDE;

public:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene) Q_DECL_OVERRIDE;
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene) Q_DECL_OVERRIDE;
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene)Q_DECL_OVERRIDE;

private:
    CGraphicsTriangleItem *m_pTriangleItem;
};

#endif // CTRIANGLETOOL_H
