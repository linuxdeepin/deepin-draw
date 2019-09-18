#ifndef CMASICOTOOL_H
#define CMASICOTOOL_H
#include "idrawtool.h"

class CGraphicsMasicoItem;
class CMasicoTool : public IDrawTool
{
public:
    CMasicoTool();
    ~CMasicoTool() Q_DECL_OVERRIDE;

public:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene) Q_DECL_OVERRIDE;
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene) Q_DECL_OVERRIDE;
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene)Q_DECL_OVERRIDE;

private:
    CGraphicsMasicoItem *m_pBlurItem;
};

#endif // CMASICOTOOL_H
