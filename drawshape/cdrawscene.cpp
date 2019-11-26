/*
 * Copyright (C) 2019 ~ %YEAR% Deepin Technology Co., Ltd.
 *
 * Author:     WangXing
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
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
#include "cgraphicsmasicoitem.h"
#include "cgraphicstextitem.h"
#include "cgraphicsproxywidget.h"
#include "cgraphicslineitem.h"
#include "cpictureitem.h"
#include "drawshape/cpictureitem.h"

#include <QGraphicsSceneMouseEvent>
#include <QDebug>
#include <QRect>
#include <QGraphicsView>
#include <QtMath>

CDrawScene *CDrawScene::m_pInstance = nullptr;

CDrawScene::CDrawScene(QObject *parent)
    : QGraphicsScene(parent)
    , m_bIsEditTextFlag(false)
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
    if (CDrawParamSigleton::GetInstance()->getRenderImage() > 0) {
        if (CDrawParamSigleton::GetInstance()->getRenderImage() == 1) {
            painter->fillRect(sceneRect(), Qt::white);
        } else {
            painter->fillRect(sceneRect(), Qt::transparent);
        }
    } else {
        if (CDrawParamSigleton::GetInstance()->getThemeType() == 1) {
            painter->fillRect(sceneRect(), Qt::white);
        } else {
            painter->fillRect(sceneRect(), QColor(55, 55, 55));
        }
    }

    /*QGraphicsScene::drawBackground(painter, rect);

    QPainterPath path;
    path.addRoundedRect(sceneRect(), 20, 20);

    int SHADOW_WIDTH = 10;
    QRectF sceneRect = this->sceneRect();
    if (CDrawParamSigleton::GetInstance()->getThemeType() == 1) {

        painter->setPen(Qt::NoPen);
        painter->fillPath(path, Qt::white);
        painter->drawPath(path);

        QColor color(50, 50, 50, 30);
        for (int i = 0; i < SHADOW_WIDTH; i++) {
            color.setAlpha(120 - qSqrt(i) * 40);
            painter->setPen(color);

            QPainterPath tmpPath;
            tmpPath.addRoundedRect(sceneRect.x() - i, sceneRect.y() - i, sceneRect.width() + 2 * i, sceneRect.height() + 2 * i, 20, 20);
            painter->drawPath(tmpPath);
            // 圆角阴影边框;
            //painter->drawRoundedRect(SHADOW_WIDTH - i, SHADOW_WIDTH - i, sceneRect().width() - (SHADOW_WIDTH - i) * 2, sceneRect().height() - (SHADOW_WIDTH - i) * 2, 4, 4);
        }
        //painter->fillRect(sceneRect(), Qt::white);
    } else {
        painter->setPen(Qt::NoPen);
        painter->fillPath(path, QColor(40, 40, 40));
        painter->drawPath(path);
    }*/
}

CDrawScene *CDrawScene::GetInstance()
{
    if (m_pInstance == nullptr) {
        m_pInstance  = new CDrawScene();
    }
    return m_pInstance;
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

            if (item->type() != BlurType) {
                if (tmpitem->pen() != CDrawParamSigleton::GetInstance()->getPen() ||
                        tmpitem->brush() != CDrawParamSigleton::GetInstance()->getBrush() ) {
                    emit itemPropertyChange(tmpitem, tmpitem->pen(), tmpitem->brush(),
                                            tmpitem->pen() != CDrawParamSigleton::GetInstance()->getPen(),
                                            tmpitem->brush() != CDrawParamSigleton::GetInstance()->getBrush());
                    tmpitem->setPen(CDrawParamSigleton::GetInstance()->getPen());
                    tmpitem->setBrush(CDrawParamSigleton::GetInstance()->getBrush());
                }
            }


            if (item->type() == TextType) {
                //字体大小和颜色 分开处理
                static_cast<CGraphicsTextItem *>(item)->setTextColor(CDrawParamSigleton::GetInstance()->getTextColor());
                //static_cast<CGraphicsTextItem *>(item)->setFont(CDrawParamSigleton::GetInstance()->getTextFont());
                //static_cast<CGraphicsTextItem *>(item)->update();
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
            } else if (item->type() == PenType) {
                CGraphicsPenItem *tmpItem = static_cast<CGraphicsPenItem *>(item);
                if (tmpItem->currentType() != CDrawParamSigleton::GetInstance()->getCurrentPenType()) {
                    emit itemPenTypeChange(tmpItem, tmpItem->currentType());
                    tmpItem->updatePenType(CDrawParamSigleton::GetInstance()->getCurrentPenType());
                }
            } else if (item->type() == BlurType) {
                CGraphicsMasicoItem *tmpItem = static_cast<CGraphicsMasicoItem *>(item);
                if (tmpItem->getBlurWidth() != CDrawParamSigleton::GetInstance()->getBlurWidth() || tmpItem->getBlurEffect() != CDrawParamSigleton::GetInstance()->getBlurEffect()) {
                    //emit itemPolygonalStarPointChange(tmpItem, tmpItem->anchorNum(), tmpItem->innerRadius());
                    tmpItem->setBlurEffect(CDrawParamSigleton::GetInstance()->getBlurEffect());
                    tmpItem->setBlurWidth(CDrawParamSigleton::GetInstance()->getBlurWidth());
                    //用于撤消
                    emit itemBlurChange(CDrawParamSigleton::GetInstance()->getBlurWidth(),
                                        CDrawParamSigleton::GetInstance()->getBlurEffect());
                    tmpItem->update();
                }
            } else if (item->type() == LineType) {
                CGraphicsLineItem *tmpItem = static_cast<CGraphicsLineItem *>(item);
                tmpItem->calcVertexes();
                ELineType type = tmpItem->getLineType();
                if (type != CDrawParamSigleton::GetInstance()->getLineType()) {
                    tmpItem->setLineType(CDrawParamSigleton::GetInstance()->getLineType());
                    tmpItem->update();
                    //REDO UNDO
                    emit itemLineTypeChange(CDrawParamSigleton::GetInstance()->getLineType());
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
        case LineType:
            CDrawParamSigleton::GetInstance()->setPen(static_cast<CGraphicsItem *>(selectedItem)->pen());
            CDrawParamSigleton::GetInstance()->setLineType(static_cast<CGraphicsLineItem *>(selectedItem)->getLineType());
            break;
        case BlurType:
            CDrawParamSigleton::GetInstance()->setBlurEffect(static_cast<CGraphicsMasicoItem *>(selectedItem)->getBlurEffect());
            CDrawParamSigleton::GetInstance()->setBlurWidth(static_cast<CGraphicsMasicoItem *>(selectedItem)->getBlurWidth());
            break;

        default:
            break;
        }
    }
    emit signalAttributeChanged(flag, selectedItem->type());
}

void CDrawScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    //判断如果点在字体内，则变为选择工具
    QPointF pos = mouseEvent->scenePos();
    CGraphicsTextItem *textItem = nullptr;
    m_bIsEditTextFlag = false;
    QList<QGraphicsItem *> items = this->selectedItems();
    foreach (QGraphicsItem *item, items) {
        if (item->type() == TextType) {
            textItem = static_cast<CGraphicsTextItem *>(item);
            m_bIsEditTextFlag = static_cast<CGraphicsTextItem *>(item)->isEditable();
            break;
        }
    }

    if (mouseEvent->button()) {
        EDrawToolMode currentMode = CDrawParamSigleton::GetInstance()->getCurrentDrawToolMode();

        IDrawTool *pTool = CDrawToolManagerSigleton::GetInstance()->getDrawTool(currentMode);
        if (currentMode == text &&  m_bIsEditTextFlag && textItem->rect().contains(pos)) {
            pTool = CDrawToolManagerSigleton::GetInstance()->getDrawTool(selection);
        }
        if ( nullptr != pTool) {
            pTool->mousePressEvent(mouseEvent, this);
        }
    }
}

void CDrawScene::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    m_bIsEditTextFlag = false;
    QList<QGraphicsItem *> items = this->selectedItems();
    foreach (QGraphicsItem *item, items) {
        if (item->type() == TextType) {
            m_bIsEditTextFlag = static_cast<CGraphicsTextItem *>(item)->isEditable();
            break;
        }
    }

    EDrawToolMode currentMode = CDrawParamSigleton::GetInstance()->getCurrentDrawToolMode();
    IDrawTool *pTool = CDrawToolManagerSigleton::GetInstance()->getDrawTool(currentMode);
    if (currentMode == text && m_bIsEditTextFlag) {
        pTool = CDrawToolManagerSigleton::GetInstance()->getDrawTool(selection);
    }
    if ( nullptr != pTool) {
        pTool->mouseMoveEvent(mouseEvent, this);
    }
}

void CDrawScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    m_bIsEditTextFlag = false;
    QList<QGraphicsItem *> items = this->selectedItems();
    foreach (QGraphicsItem *item, items) {
        if (item->type() == TextType) {
            m_bIsEditTextFlag = static_cast<CGraphicsTextItem *>(item)->isEditable();
            break;
        }
    }

    EDrawToolMode currentMode = CDrawParamSigleton::GetInstance()->getCurrentDrawToolMode();

    IDrawTool *pTool = CDrawToolManagerSigleton::GetInstance()->getDrawTool(currentMode);
    if (currentMode == text && m_bIsEditTextFlag) {
        pTool = CDrawToolManagerSigleton::GetInstance()->getDrawTool(selection);
    }
    if ( nullptr != pTool) {
        pTool->mouseReleaseEvent(mouseEvent, this);
//        if (pTool->getDrawToolMode() != cut) {
//            CDrawParamSigleton::GetInstance()->setCurrentDrawToolMode(selection);
//            emit signalChangeToSelect();
//        }
    }
}

void CDrawScene::drawItems(QPainter *painter, int numItems, QGraphicsItem *items[], const QStyleOptionGraphicsItem options[], QWidget *widget)
{
    painter->setClipping(true);
    painter->setClipRect(sceneRect());

    QGraphicsScene::drawItems(painter, numItems, items, options, widget);
}

void CDrawScene::showCutItem()
{
    EDrawToolMode currentMode = CDrawParamSigleton::GetInstance()->getCurrentDrawToolMode();
    setItemDisable(false);
    IDrawTool *pTool = CDrawToolManagerSigleton::GetInstance()->getDrawTool(currentMode);
    if ( nullptr != pTool && cut == pTool->getDrawToolMode()) {
        static_cast<CCutTool *>(pTool)->createCutItem(this);
        emit signalUpdateCutSize();
    }
}

void CDrawScene::quitCutMode()
{
    EDrawToolMode mode = CDrawParamSigleton::GetInstance()->getCurrentDrawToolMode();
    if (cut == mode) {
        IDrawTool *pTool = CDrawToolManagerSigleton::GetInstance()->getDrawTool(mode);
        if (nullptr != pTool) {
            static_cast<CCutTool *>(pTool)->deleteCutItem(this);
            setItemDisable(true);
            CDrawParamSigleton::GetInstance()->setCurrentDrawToolMode(selection);
            emit signalQuitCutAndChangeToSelect();
        }
    }
}

void CDrawScene::doCutScene()
{
    EDrawToolMode mode = CDrawParamSigleton::GetInstance()->getCurrentDrawToolMode();
    if (cut == mode) {
        IDrawTool *pTool = CDrawToolManagerSigleton::GetInstance()->getDrawTool(mode);
        if (nullptr != pTool) {
            QRectF rect = static_cast<CCutTool *>(pTool)->getCutRect();
            setSceneRect(rect);
            update();
            quitCutMode();
        }
    }

    //更新模糊图元
    QList<QGraphicsItem *> items = this->items(this->sceneRect());

    foreach (QGraphicsItem *item, items) {
        if (item->type() == BlurType) {
            static_cast<CGraphicsMasicoItem *>(item)->setPixmap();
        }
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

void CDrawScene::drawToolChange(int type)
{
    Q_UNUSED(type)
    this->clearSelection();
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

void CDrawScene::textFontFamilyChanged()
{
    QList<QGraphicsItem *> items = this->selectedItems();

    QGraphicsItem *item = nullptr;
    foreach (item, items) {
        if (item->type() == TextType) {
            CGraphicsTextItem *tmpitem = static_cast<CGraphicsTextItem *>(item);
            tmpitem->setFontFamily(CDrawParamSigleton::GetInstance()->getTextFont().family());
        }
    }
}

void CDrawScene::textFontSizeChanged()
{
    QList<QGraphicsItem *> items = this->selectedItems();

    QGraphicsItem *item = nullptr;
    foreach (item, items) {
        if (item->type() == TextType) {
            CGraphicsTextItem *tmpitem = static_cast<CGraphicsTextItem *>(item);
            tmpitem->setFontSize(CDrawParamSigleton::GetInstance()->getTextFont().pointSizeF());
        }
    }
}

void CDrawScene::updateBlurItem()
{
    QList<QGraphicsItem *> items = this->items();
    foreach (QGraphicsItem *item, items) {
        if (item->type() == BlurType) {
            static_cast<CGraphicsMasicoItem *>(item)->setPixmap();
        }
    }
}

void CDrawScene::switchTheme(int type)
{
    QList<QGraphicsItem *> items = CDrawScene::GetInstance()->items();//this->collidingItems();
    //QList<QGraphicsItem *> items = this->collidingItems();
    for (int i = items.size() - 1; i >= 0; i-- ) {
        if (items[i]->type() == BlurType) {
            static_cast<CGraphicsMasicoItem *>(items[i])->setPixmap();
        }
    }
}

