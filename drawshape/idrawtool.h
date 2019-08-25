#ifndef CDRAWTOOL_H
#define CDRAWTOOL_H

#include "globaldefine.h"
#include <QList>

class QGraphicsSceneMouseEvent;
class CDrawScene;

class IDrawTool
{
public:
    IDrawTool(EDrawToolMode mode);
    virtual ~IDrawTool() = 0;
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene) = 0;
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene) = 0;
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene) = 0;

    EDrawToolMode getDrawToolMode() const;

protected:
    bool m_bMousePress;
    QPointF m_sPointPress;
    QPointF m_sPointRelease;


private:
    EDrawToolMode m_mode;

};

#endif // CDRAWTOOL_H
