#ifndef CLINETOOL_H
#define CLINETOOL_H
#include "idrawtool.h"
#include "cgraphicslineitem.h"

class CLineTool : public IDrawTool
{
public:
    CLineTool();
    virtual ~CLineTool() Q_DECL_OVERRIDE;

public:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene) Q_DECL_OVERRIDE;
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene) Q_DECL_OVERRIDE;
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene)Q_DECL_OVERRIDE;

private:
    CGraphicsLineItem *m_pLineItem;
};

#endif // CLINETOOL_H
