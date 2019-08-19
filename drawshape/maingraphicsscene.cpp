#include "maingraphicsscene.h"
#include "drawtool.h"
#include <QGraphicsSceneMouseEvent>

MainGraphicsScene::MainGraphicsScene(QObject *parent) : QGraphicsScene(parent)
{

}

void MainGraphicsScene::setView(QGraphicsView *view)
{
    m_graphicsView = view ;
}

QGraphicsView *MainGraphicsScene::view()
{
    return m_graphicsView;
}

void MainGraphicsScene::mouseEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    switch ( mouseEvent->type() ) {
    case QEvent::GraphicsSceneMousePress:
        QGraphicsScene::mousePressEvent(mouseEvent);
        break;
    case QEvent::GraphicsSceneMouseMove:
        QGraphicsScene::mouseMoveEvent(mouseEvent);
        break;
    case QEvent::GraphicsSceneMouseRelease:
        QGraphicsScene::mouseReleaseEvent(mouseEvent);
        break;
    }
}

void MainGraphicsScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    DrawTool *tool = DrawTool::findTool( DrawTool::c_drawShape );
    if ( tool ) {
        tool->mousePressEvent(mouseEvent, this);
    }

}

void MainGraphicsScene::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    DrawTool *tool = DrawTool::findTool( DrawTool::c_drawShape );
    if ( tool ) {
        tool->mouseMoveEvent(mouseEvent, this);
    }
}

void MainGraphicsScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    DrawTool *tool = DrawTool::findTool( DrawTool::c_drawShape );
    if ( tool ) {
        tool->mouseReleaseEvent(mouseEvent, this);
    }
}
