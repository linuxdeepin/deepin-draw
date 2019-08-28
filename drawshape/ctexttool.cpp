#include "ctexttool.h"
#include "cgraphicstextitem.h"
#include "cdrawscene.h"
#include <QGraphicsSceneMouseEvent>
CTextTool::CTextTool()
    : IDrawTool(text)
{

}

CTextTool::~CTextTool()
{

}

void CTextTool::mousePressEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene)
{
    Q_UNUSED(event)
    CGraphicsTextItem *item = new CGraphicsTextItem();
    item->setPos(event->scenePos());
    scene->addItem(item);
    item->setFocus();
    //item->setPos(scene->pos());
}

void CTextTool::mouseMoveEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene)
{
    Q_UNUSED(scene)
    scene->mouseEvent(event);
}

void CTextTool::mouseReleaseEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene)
{
    Q_UNUSED(scene)
    scene->mouseEvent(event);
}
