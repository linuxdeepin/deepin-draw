#include "cdrawscene.h"
#include "cgraphicsrectitem.h"
#include "idrawtool.h"
#include "cdrawtoolmanagersigleton.h"
#include "cdrawparamsigleton.h"
#include "globaldefine.h"
#include "cgraphicspolygonitem.h"
#include "cgraphicspolygonalstaritem.h"
#include "cgraphicstextitem.h"
#include "cgraphicspenitem.h"
#include "frame/cpicturewidget.h"

#include <QGraphicsSceneMouseEvent>
#include <QDebug>
#include <QRect>
#include <QGraphicsView>
#include <drawshape/cpictureitem.h>


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

void CDrawScene::drawBackground(QPainter *painter, const QRectF &rect)
{
    QGraphicsScene::drawBackground(painter, rect);
    painter->fillRect(sceneRect(), Qt::white);
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
        if (item->type() != TextType) {
            static_cast<CGraphicsItem *>(item)->setPen(CDrawParamSigleton::GetInstance()->getPen());
            static_cast<CGraphicsItem *>(item)->setBrush(CDrawParamSigleton::GetInstance()->getBrush());
        }

        if (item->type() == TextType) {
            static_cast<CGraphicsTextItem *>(item)->setFont(CDrawParamSigleton::GetInstance()->getTextFont());
        } else if (item->type() == PolygonType) {
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
        //排除文字图元
        if (selectedItem->type() != TextType) {
            CDrawParamSigleton::GetInstance()->setPen(static_cast<CGraphicsItem *>(selectedItem)->pen());
            CDrawParamSigleton::GetInstance()->setBrush(static_cast<CGraphicsItem *>(selectedItem)->brush());
        }
        ///特殊属性图元 读取额外的特殊属性并设置到全局属性中
        if (selectedItem->type() == PolygonType) {
            CDrawParamSigleton::GetInstance()->setSideNum(static_cast<CGraphicsPolygonItem *>(selectedItem)->nPointsCount());
        } else if (selectedItem->type() == PolygonalStarType) {
            CDrawParamSigleton::GetInstance()->setAnchorNum(static_cast<CGraphicsPolygonalStarItem *>(selectedItem)->anchorNum());
            CDrawParamSigleton::GetInstance()->setRadiusNum(static_cast<CGraphicsPolygonalStarItem *>(selectedItem)->innerRadius());
        } else if (selectedItem->type() ==  PenType) {
            CDrawParamSigleton::GetInstance()->setCurrentPenType(static_cast<CGraphicsPenItem *>(selectedItem)->currentType());
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
    CDrawParamSigleton::GetInstance()->setCurrentDrawToolMode(selection);
    emit signalChangeToSelect();
}



void CDrawScene::picOperation(int enumstyle)
{

    qDebug() << "entered the  picOperation function" << endl;
    QList<QGraphicsItem *> items = this->selectedItems();
    if ( items.count() != 0 ) {
        QGraphicsItem *item = items.first();
        //需要区别图元或文字
        if (item->type() == PictureType) {
            CPictureItem *pictureItem = static_cast<CPictureItem *>(item);
            if (pictureItem != nullptr) {
                switch (enumstyle) {
                case CPictureWidget::LeftRotate:
                    pictureItem->setRotation90(true);
                    break;
                case CPictureWidget::RightRotate:
                    pictureItem->setRotation90(false);
                    break;
                case CPictureWidget::FlipHorizontal:
                    pictureItem->setMirror(false, true);
                    break;
                case CPictureWidget::FlipVertical:
                    pictureItem->setMirror(true, false);
                    break;
                default:
                    break;
                }

            }
        }

    }

}

