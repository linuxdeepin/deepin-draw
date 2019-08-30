#include "cdrawscene.h"
#include "cgraphicsrectitem.h"
#include "idrawtool.h"
#include "cdrawtoolmanagersigleton.h"
#include "cdrawparamsigleton.h"
#include "globaldefine.h"
#include "cgraphicspolygonitem.h"
#include "cgraphicspolygonalstaritem.h"
#include <QGraphicsSceneMouseEvent>
#include <QDebug>
#include <QRect>
#include <QGraphicsView>

CDrawScene::CDrawScene(QObject *parent)
    : QGraphicsScene(parent)
{

}

void CDrawScene::mouseEvent(QGraphicsSceneMouseEvent *mouseEvent)
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
    default:
        break;
    }
}

void CDrawScene::keyEvent(QKeyEvent *keyEvent)
{
    switch ( keyEvent->type() ) {
    case QEvent::KeyPress:                   // mouse button pressed
        QGraphicsScene::keyPressEvent(keyEvent);
        break;
    case QEvent::KeyRelease:
        QGraphicsScene::keyReleaseEvent(keyEvent);
        break;
    default:
        break;
    }
}

void CDrawScene::setCursor(const QCursor &cursor)
{
    QList<QGraphicsView *> views  = this->views();
    if ( views.count() > 0 ) {
        QGraphicsView *view = views.first();
        view->setCursor(cursor);
    }
}

void CDrawScene::attributeChanged()
{
    QList<QGraphicsItem *> items = this->selectedItems();

    QGraphicsItem *item = nullptr;
    foreach (item, items) {
        static_cast<CGraphicsItem *>(item)->setPen(CDrawParamSigleton::GetInstance()->getPen());
        static_cast<CGraphicsItem *>(item)->setBrush(CDrawParamSigleton::GetInstance()->getBrush());
        if (item->type() == PolygonType) {
            static_cast<CGraphicsPolygonItem *>(item)->setPointCount(CDrawParamSigleton::GetInstance()->getSideNum());
        } else if (item->type() == PolygonalStarType) {
            static_cast<CGraphicsPolygonalStarItem *>(item)->updatePolygonalStar(CDrawParamSigleton::GetInstance()->getAnchorNum(),
                                                                                 CDrawParamSigleton::GetInstance()->getRadiusNum());
        }
    }
}

void CDrawScene::changeAttribute(bool flag, QGraphicsItem *selectedItem)
{
    if (flag) {
        CDrawParamSigleton::GetInstance()->setPen(static_cast<CGraphicsItem *>(selectedItem)->pen());
        CDrawParamSigleton::GetInstance()->setBrush(static_cast<CGraphicsItem *>(selectedItem)->brush());
        ///特殊属性图元 读取额外的特殊属性并设置到全局属性中
        if (selectedItem->type() == PolygonType) {
            CDrawParamSigleton::GetInstance()->setSideNum(static_cast<CGraphicsPolygonItem *>(selectedItem)->nPointsCount());
        } else if (selectedItem->type() == PolygonalStarType) {
            CDrawParamSigleton::GetInstance()->setAnchorNum(static_cast<CGraphicsPolygonalStarItem *>(selectedItem)->anchorNum());
            CDrawParamSigleton::GetInstance()->setRadiusNum(static_cast<CGraphicsPolygonalStarItem *>(selectedItem)->innerRadius());
        }
    }
    emit signalAttributeChanged(flag, selectedItem->type());
}

void CDrawScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    EDrawToolMode currentMode = CDrawParamSigleton::GetInstance()->getCurrentDrawToolMode();
    IDrawTool *pTool = CDrawToolManagerSigleton::GetInstance()->getDrawTool(currentMode);
    if ( nullptr != pTool ) {
        pTool->mousePressEvent(mouseEvent, this);
    }
}

void CDrawScene::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    EDrawToolMode currentMode = CDrawParamSigleton::GetInstance()->getCurrentDrawToolMode();
    IDrawTool *pTool = CDrawToolManagerSigleton::GetInstance()->getDrawTool(currentMode);
    if ( nullptr != pTool ) {
        pTool->mouseMoveEvent(mouseEvent, this);
    }
}

void CDrawScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    EDrawToolMode currentMode = CDrawParamSigleton::GetInstance()->getCurrentDrawToolMode();
    IDrawTool *pTool = CDrawToolManagerSigleton::GetInstance()->getDrawTool(currentMode);
    if ( nullptr != pTool ) {
        pTool->mouseReleaseEvent(mouseEvent, this);
    }
}

void CDrawScene::keyPressEvent(QKeyEvent *event)
{
    EDrawToolMode currentMode = CDrawParamSigleton::GetInstance()->getCurrentDrawToolMode();
    IDrawTool *pTool = CDrawToolManagerSigleton::GetInstance()->getDrawTool(currentMode);
    if ( nullptr != pTool ) {
        pTool->keyPressEvent(event, this);
    }
}

void CDrawScene::keyReleaseEvent(QKeyEvent *event)
{
    EDrawToolMode currentMode = CDrawParamSigleton::GetInstance()->getCurrentDrawToolMode();
    IDrawTool *pTool = CDrawToolManagerSigleton::GetInstance()->getDrawTool(currentMode);
    if ( nullptr != pTool ) {
        pTool->keyReleaseEvent(event, this);
    }
}
