#ifndef CSELECTTOOL_H
#define CSELECTTOOL_H
#include "idrawtool.h"
#include "globaldefine.h"
#include "csizehandlerect.h"

#include <QCursor>

class CGraphicsItem;
class CGraphicsRotateAngleItem;

class CSelectTool : public IDrawTool
{
public:
    CSelectTool();
    virtual ~CSelectTool();
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene);

private:
    CGraphicsItem *m_currentSelectItem;
    CSizeHandleRect::EDirection m_dragHandle; //选中的方块方向
    bool m_bRotateAng;
    qreal m_rotateAng;
    QCursor m_RotateCursor;
    QPointF m_initRotateItemPos;
    CGraphicsRotateAngleItem *m_RotateItem;
};

#endif // CSELECTTOOL_H
