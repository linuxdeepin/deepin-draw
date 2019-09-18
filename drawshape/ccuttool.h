#ifndef CCUTTOOL_H
#define CCUTTOOL_H

#include "idrawtool.h"
#include "cgraphicscutitem.h"
#include "csizehandlerect.h"
#include "cbuttonrect.h"


class CCutTool : public IDrawTool
{
public:
    CCutTool();
    virtual ~CCutTool() Q_DECL_OVERRIDE;

public:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene) Q_DECL_OVERRIDE;
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene) Q_DECL_OVERRIDE;
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene)Q_DECL_OVERRIDE;
    void createCutItem(CDrawScene *scene);
    void deleteCutItem(CDrawScene *scene);
    void changeCutType(int, CDrawScene *scene);
    void changeCutSize(const QSize &);

private:
    CGraphicsCutItem *m_pCutItem;
    CSizeHandleRect::EDirection m_dragHandle; //选中的方块方向
    CButtonRect::EButtonType m_buttonType;

};

#endif // CCUTTOOL_H
