#include "cselecttool.h"
#include "cdrawscene.h"
CSelectTool::CSelectTool ()
    : IDrawTool (selection)
{

}

CSelectTool::~CSelectTool()
{

}

void CSelectTool::mousePressEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene)
{
    scene->mouseEvent(event);
}

void CSelectTool::mouseMoveEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene)
{
    scene->mouseEvent(event);
}

void CSelectTool::mouseReleaseEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene)
{
    scene->mouseEvent(event);
}


