#include "cdrawscene.h"
#include "cgraphicsrectitem.h"
#include "idrawtool.h"
#include "cdrawtoolmanagersigleton.h"
#include "cdrawparamsigleton.h"
#include "globaldefine.h"
#include "cgraphicspolygonitem.h"
#include "cgraphicspolygonalstaritem.h"
#include "cgraphicspenitem.h"
#include "frame/cpicturewidget.h"
#include "cgraphicstextitem.h"
#include "ccuttool.h"


#include <QGraphicsSceneMouseEvent>
#include <QDebug>
#include <QRect>
#include <QGraphicsView>
#include <drawshape/cpictureitem.h>


CDrawScene::CDrawScene(QObject *parent)
    : QGraphicsScene(parent)
{
    connect(this, SIGNAL(selectionChanged()), this, SLOT(slotSelectionChange()));
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
    EDrawToolMode currentMode = CDrawParamSigleton::GetInstance()->getCurrentDrawToolMode();
    ///区分裁剪
    if (cut == currentMode) {
        ECutAttributeType attributeType = CDrawParamSigleton::GetInstance()->getCutAttributeType();
        IDrawTool *pTool = CDrawToolManagerSigleton::GetInstance()->getDrawTool(cut);
        if (attributeType == ECutAttributeType::ButtonClickAttribute) {
            if (nullptr != pTool) {
                static_cast<CCutTool *>(pTool)->changeCutType(CDrawParamSigleton::GetInstance()->getCutType(), this);
            }
        } else if (attributeType == ECutAttributeType::LineEditeAttribute) {
            if (nullptr != pTool) {
                static_cast<CCutTool *>(pTool)->changeCutSize(CDrawParamSigleton::GetInstance()->getCutSize());
            }
        }
    } else {
        QList<QGraphicsItem *> items = this->selectedItems();

        QGraphicsItem *item = nullptr;
        foreach (item, items) {
            CGraphicsItem *tmpitem = static_cast<CGraphicsItem *>(item);

            if (tmpitem->pen() != CDrawParamSigleton::GetInstance()->getPen() ||
                    tmpitem->brush() != CDrawParamSigleton::GetInstance()->getBrush() ) {
                emit itemPropertyChange(tmpitem, tmpitem->pen(), tmpitem->brush(),
                                        tmpitem->pen() != CDrawParamSigleton::GetInstance()->getPen(),
                                        tmpitem->brush() != CDrawParamSigleton::GetInstance()->getBrush());
                tmpitem->setPen(CDrawParamSigleton::GetInstance()->getPen());
                tmpitem->setBrush(CDrawParamSigleton::GetInstance()->getBrush());
            }


            if (item->type() == TextType) {
                static_cast<CGraphicsTextItem *>(item)->setTextColor(CDrawParamSigleton::GetInstance()->getTextColor());
                static_cast<CGraphicsTextItem *>(item)->setFont(CDrawParamSigleton::GetInstance()->getTextFont());
                //static_cast<CGraphicsTextItem *>(item)->setFontSize(CDrawParamSigleton::GetInstance()->getTextSize());
            } else if (item->type() == PolygonType) {
                if (CDrawParamSigleton::GetInstance()->getSideNum() != static_cast<CGraphicsPolygonItem *>(item)->nPointsCount()) {
                    emit itemPolygonPointChange(static_cast<CGraphicsPolygonItem *>(item), static_cast<CGraphicsPolygonItem *>(item)->nPointsCount());
                    static_cast<CGraphicsPolygonItem *>(item)->setPointCount(CDrawParamSigleton::GetInstance()->getSideNum());
                }
            } else if (item->type() == PolygonalStarType) {
                CGraphicsPolygonalStarItem *tmpItem = static_cast<CGraphicsPolygonalStarItem *>(item);
                if (tmpItem->anchorNum() != CDrawParamSigleton::GetInstance()->getAnchorNum() || tmpItem->innerRadius() != CDrawParamSigleton::GetInstance()->getRadiusNum()) {
                    emit itemPolygonalStarPointChange(tmpItem, tmpItem->anchorNum(), tmpItem->innerRadius());
                    tmpItem->updatePolygonalStar(CDrawParamSigleton::GetInstance()->getAnchorNum(),
                                                 CDrawParamSigleton::GetInstance()->getRadiusNum());
                }
            }
        }
    }
}

void CDrawScene::changeAttribute(bool flag, QGraphicsItem *selectedItem)
{
    if (flag) {
        switch (selectedItem->type()) {
        case RectType:
        case EllipseType:
        case TriangleType:
            CDrawParamSigleton::GetInstance()->setPen(static_cast<CGraphicsItem *>(selectedItem)->pen());
            CDrawParamSigleton::GetInstance()->setBrush(static_cast<CGraphicsItem *>(selectedItem)->brush());
            break;
        case PolygonType:
            CDrawParamSigleton::GetInstance()->setPen(static_cast<CGraphicsItem *>(selectedItem)->pen());
            CDrawParamSigleton::GetInstance()->setBrush(static_cast<CGraphicsItem *>(selectedItem)->brush());
            CDrawParamSigleton::GetInstance()->setSideNum(static_cast<CGraphicsPolygonItem *>(selectedItem)->nPointsCount());
            break;
        case PolygonalStarType:
            CDrawParamSigleton::GetInstance()->setPen(static_cast<CGraphicsItem *>(selectedItem)->pen());
            CDrawParamSigleton::GetInstance()->setBrush(static_cast<CGraphicsItem *>(selectedItem)->brush());
            CDrawParamSigleton::GetInstance()->setAnchorNum(static_cast<CGraphicsPolygonalStarItem *>(selectedItem)->anchorNum());
            CDrawParamSigleton::GetInstance()->setRadiusNum(static_cast<CGraphicsPolygonalStarItem *>(selectedItem)->innerRadius());
            break;
        case PenType:
            CDrawParamSigleton::GetInstance()->setPen(static_cast<CGraphicsItem *>(selectedItem)->pen());
            CDrawParamSigleton::GetInstance()->setCurrentPenType(static_cast<CGraphicsPenItem *>(selectedItem)->currentType());
            break;
        default:
            break;
        }
    }
    emit signalAttributeChanged(flag, selectedItem->type());
}

void CDrawScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if (mouseEvent->button() & Qt::LeftButton) {
        EDrawToolMode currentMode = CDrawParamSigleton::GetInstance()->getCurrentDrawToolMode();

        IDrawTool *pTool = CDrawToolManagerSigleton::GetInstance()->getDrawTool(currentMode);
        if ( nullptr != pTool) {
            pTool->mousePressEvent(mouseEvent, this);
        }
    }
}

void CDrawScene::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    EDrawToolMode currentMode = CDrawParamSigleton::GetInstance()->getCurrentDrawToolMode();
    IDrawTool *pTool = CDrawToolManagerSigleton::GetInstance()->getDrawTool(currentMode);
    if ( nullptr != pTool) {
        pTool->mouseMoveEvent(mouseEvent, this);
    }
}

void CDrawScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    EDrawToolMode currentMode = CDrawParamSigleton::GetInstance()->getCurrentDrawToolMode();
    IDrawTool *pTool = CDrawToolManagerSigleton::GetInstance()->getDrawTool(currentMode);
    if ( nullptr != pTool) {
        pTool->mouseReleaseEvent(mouseEvent, this);
        if (pTool->getDrawToolMode() != cut) {
            CDrawParamSigleton::GetInstance()->setCurrentDrawToolMode(selection);
            emit signalChangeToSelect();
        }
    }
}

void CDrawScene::drawItems(QPainter *painter, int numItems, QGraphicsItem *items[], const QStyleOptionGraphicsItem options[], QWidget *widget)
{
    QRectF rect = this->sceneRect();
    painter->setClipping(true);
    painter->setClipRect(rect);
    QGraphicsScene::drawItems(painter, numItems, items, options, widget);
}

void CDrawScene::showCutItem()
{
    EDrawToolMode currentMode = CDrawParamSigleton::GetInstance()->getCurrentDrawToolMode();
    setItemDisable(false);
    IDrawTool *pTool = CDrawToolManagerSigleton::GetInstance()->getDrawTool(currentMode);
    if ( nullptr != pTool && cut == pTool->getDrawToolMode()) {
        static_cast<CCutTool *>(pTool)->createCutItem(this);
        signalUpdateCutSize();
    }
}

void CDrawScene::quitCutMode()
{
    IDrawTool *pTool = CDrawToolManagerSigleton::GetInstance()->getDrawTool(cut);
    if (nullptr != pTool) {
        static_cast<CCutTool *>(pTool)->deleteCutItem(this);
        setItemDisable(true);
        CDrawParamSigleton::GetInstance()->setCurrentDrawToolMode(selection);
        emit signalQuitCutMode();
    }
}


void CDrawScene::picOperation(int enumstyle)
{

    //qDebug() << "entered the  picOperation function" << endl;
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
                    pictureItem->setMirror(true, false);
                    break;
                case CPictureWidget::FlipVertical:
                    pictureItem->setMirror(false, true);
                    break;
                default:
                    break;
                }

            }
        }

    }
}

void CDrawScene::slotSelectionChange()
{
    IDrawTool *pTool = CDrawToolManagerSigleton::GetInstance()->getDrawTool(selection);
    if ( nullptr != pTool ) {
        static_cast<CSelectTool *>(pTool)->selectionChange();
    }
}

void CDrawScene::setItemDisable(bool canSelecte)
{
    ///让其他图元不可选中
    QList<QGraphicsItem *> itemList = this->items();
    foreach (QGraphicsItem *item, itemList) {
        if (item->type() > QGraphicsItem::UserType) {
            item->setFlag(QGraphicsItem::ItemIsMovable, canSelecte);
            item->setFlag(QGraphicsItem::ItemIsSelectable, canSelecte);
        }
    }
}
