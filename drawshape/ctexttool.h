#ifndef CTEXTTOOL_H
#define CTEXTTOOL_H
#include "idrawtool.h"

class CTextTool : public IDrawTool
{
public:
    CTextTool();
    virtual ~CTextTool() Q_DECL_OVERRIDE;

public:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene) Q_DECL_OVERRIDE;
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene) Q_DECL_OVERRIDE;
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene) Q_DECL_OVERRIDE;
};

#endif // CTEXTTOOL_H
