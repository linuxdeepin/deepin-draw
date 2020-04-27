/*
 * Copyright (C) 2019 ~ %YEAR% Deepin Technology Co., Ltd.
 *
 * Author:     WangXin
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
#include "frame/cviewmanagement.h"
#include "frame/cgraphicsview.h"

#include <QGraphicsSceneMouseEvent>
#include <QDebug>
#include <QRect>
#include <QGraphicsView>
#include <QtMath>
#include <DApplication>

CDrawScene *CDrawScene::m_pInstance = nullptr;

CDrawScene::CDrawScene(QObject *parent)
    : QGraphicsScene(parent)
    , m_bIsEditTextFlag(false)
    , m_drawMouse(QPixmap(":/icons/draw_mouse.svg"))
    , m_lineMouse(QPixmap(":/icons/line_mouse.svg"))
    , m_pengatonMouse(QPixmap(":/icons/pengaton_mouse.svg"))
    , m_rectangleMouse(QPixmap(":/icons/rectangle_mouse.svg"))
    , m_roundMouse(QPixmap(":/icons/round_mouse.svg"))
    , m_starMouse(QPixmap(":/icons/star_mouse.svg"))
    , m_triangleMouse(QPixmap(":/icons/triangle_mouse.svg"))
    , m_textMouse(QPixmap(":/icons/text_mouse.svg"), 3, 2)
    , m_brushMouse(QPixmap(":/icons/brush_mouse.svg"), 7, 26)
    , m_blurMouse(QPixmap(":/icons/smudge_mouse.png"))
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
    if (!CManageViewSigleton::GetInstance()->getCurView()) {
        return;
    }
    QGraphicsScene::drawBackground(painter, rect);
    if (CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getRenderImage() > 0) {
        if (CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getRenderImage() == 1) {
            painter->fillRect(sceneRect(), Qt::white);
        } else {
            painter->fillRect(sceneRect(), Qt::transparent);
        }
    } else {
        if (CManageViewSigleton::GetInstance()->getThemeType() == 1) {
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

void CDrawScene::resetSceneBackgroundBrush()
{
    int themeType = CManageViewSigleton::GetInstance()->getThemeType();
    if (themeType == 1) {
        this->setBackgroundBrush(QColor(248, 248, 251));
    } else if (themeType == 2) {
        this->setBackgroundBrush(QColor(35, 35, 35));
    }
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
    EDrawToolMode currentMode = CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getCurrentDrawToolMode();
    ///区分裁剪
    if (cut == currentMode) {
        ECutAttributeType attributeType = CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getCutAttributeType();
        IDrawTool *pTool = CDrawToolManagerSigleton::GetInstance()->getDrawTool(cut);
        if (attributeType == ECutAttributeType::ButtonClickAttribute) {
            if (nullptr != pTool) {
                static_cast<CCutTool *>(pTool)->changeCutType(CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getCutType(), this);
            }
        } else if (attributeType == ECutAttributeType::LineEditeAttribute) {
            if (nullptr != pTool) {
                static_cast<CCutTool *>(pTool)->changeCutSize(CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getCutSize());
            }
        }
    } else {
        QList<QGraphicsItem *> items = this->selectedItems();

        QGraphicsItem *item = nullptr;
        foreach (item, items) {
            CGraphicsItem *tmpitem = static_cast<CGraphicsItem *>(item);

            if (item->type() != BlurType) {
                if (tmpitem->pen() != CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getPen() ||
                        tmpitem->brush() != CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getBrush() ) {
                    emit itemPropertyChange(tmpitem, CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getPen(), CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getBrush(),
                                            tmpitem->pen() != CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getPen(),
                                            tmpitem->brush() != CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getBrush());
//                    tmpitem->setPen(CDrawParamSigleton::GetInstance()->getPen());
//                    tmpitem->setBrush(CDrawParamSigleton::GetInstance()->getBrush());
                }
            }


            if (item->type() == TextType) {
                //字体大小和颜色 分开处理
                static_cast<CGraphicsTextItem *>(item)->setTextColor(CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getTextColor());

            } else if (item->type() == PolygonType) {
                if (CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getSideNum() != static_cast<CGraphicsPolygonItem *>(item)->nPointsCount()) {
                    emit itemPolygonPointChange(static_cast<CGraphicsPolygonItem *>(item), CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getSideNum());
//                    static_cast<CGraphicsPolygonItem *>(item)->setPointCount(CDrawParamSigleton::GetInstance()->getSideNum());
                }
            } else if (item->type() == PolygonalStarType) {
                CGraphicsPolygonalStarItem *tmpItem = static_cast<CGraphicsPolygonalStarItem *>(item);
                if (tmpItem->anchorNum() != CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getAnchorNum() || tmpItem->innerRadius() != CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getRadiusNum()) {

//                    int oldAnchorNum = tmpItem->anchorNum();
//                    int oldRadius = tmpItem->innerRadius();
//                    tmpItem->updatePolygonalStar(CDrawParamSigleton::GetInstance()->getAnchorNum(),
//                                                 CDrawParamSigleton::GetInstance()->getRadiusNum());
                    emit itemPolygonalStarPointChange(tmpItem, CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getAnchorNum(), CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getRadiusNum());
                }
            } else if (item->type() == PenType) {
                CGraphicsPenItem *tmpItem = static_cast<CGraphicsPenItem *>(item);
                if (tmpItem->currentType() != CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getCurrentPenType()) {
                    emit itemPenTypeChange(tmpItem, CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getCurrentPenType());
                    //tmpItem->updatePenType(CDrawParamSigleton::GetInstance()->getCurrentPenType());
                }
                tmpItem->calcVertexes();
            } else if (item->type() == BlurType) {
                CGraphicsMasicoItem *tmpItem = static_cast<CGraphicsMasicoItem *>(item);
                if (tmpItem->getBlurWidth() != CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getBlurWidth() || tmpItem->getBlurEffect() != CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getBlurEffect()) {
                    //emit itemPolygonalStarPointChange(tmpItem, tmpItem->anchorNum(), tmpItem->innerRadius());
//                    tmpItem->setBlurEffect(CDrawParamSigleton::GetInstance()->getBlurEffect());
//                    tmpItem->setBlurWidth(CDrawParamSigleton::GetInstance()->getBlurWidth());
                    //用于撤消
                    emit itemBlurChange(tmpItem, (int)CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getBlurEffect(), CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getBlurWidth());
                    tmpItem->update();
                }
            } else if (item->type() == LineType) {
                CGraphicsLineItem *tmpItem = static_cast<CGraphicsLineItem *>(item);
                tmpItem->calcVertexes();
                ELineType type = tmpItem->getLineType();
                if (type != CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getLineType()) {
                    //tmpItem->setLineType(CDrawParamSigleton::GetInstance()->getLineType());
                    tmpItem->update();
                    //REDO UNDO
                    emit itemLineTypeChange(tmpItem, CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getLineType());
                    tmpItem->update();
                }

            }
        }
    }
    CDrawScene::GetInstance()->renderSelfToPixmap();
}

void CDrawScene::changeAttribute(bool flag, QGraphicsItem *selectedItem)
{
    QGraphicsItem *tmpItem = selectedItem;
    QList<QGraphicsItem *> items = this->selectedItems();
    int count = items.count();
    //多选状态
    if (count > 1) {
        CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setSelectAllFlag(true);
        if (flag) {
            emit signalAttributeChanged(flag, RectType);
        }

    } else if (count == 1) {
        if (selectedItem == nullptr) {
            tmpItem = items[0];
        }

        if (flag) {
            switch (tmpItem->type()) {
            case RectType:
            case EllipseType:
            case TriangleType:
                CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setPen(static_cast<CGraphicsItem *>(tmpItem)->pen());
                CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setBrush(static_cast<CGraphicsItem *>(tmpItem)->brush());
                break;
            case PolygonType:
                CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setPen(static_cast<CGraphicsItem *>(tmpItem)->pen());
                CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setBrush(static_cast<CGraphicsItem *>(tmpItem)->brush());
                CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setSideNum(static_cast<CGraphicsPolygonItem *>(tmpItem)->nPointsCount());
                break;
            case PolygonalStarType:
                CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setPen(static_cast<CGraphicsItem *>(tmpItem)->pen());
                CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setBrush(static_cast<CGraphicsItem *>(tmpItem)->brush());
                CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setAnchorNum(static_cast<CGraphicsPolygonalStarItem *>(tmpItem)->anchorNum());
                CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setRadiusNum(static_cast<CGraphicsPolygonalStarItem *>(tmpItem)->innerRadius());
                break;
            case PenType:
                CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setPen(static_cast<CGraphicsItem *>(tmpItem)->pen());
                CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setCurrentPenType(static_cast<CGraphicsPenItem *>(tmpItem)->currentType());
                break;
            case LineType:
                CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setPen(static_cast<CGraphicsItem *>(tmpItem)->pen());
                CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setLineType(static_cast<CGraphicsLineItem *>(tmpItem)->getLineType());
                break;
            case BlurType:
                CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setBlurEffect(static_cast<CGraphicsMasicoItem *>(tmpItem)->getBlurEffect());
                CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setBlurWidth(static_cast<CGraphicsMasicoItem *>(tmpItem)->getBlurWidth());
                break;

            default:
                break;
            }
        }
        emit signalAttributeChanged(flag, tmpItem->type());
    }
}

void CDrawScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    emit signalUpdateColorPanelVisible(mouseEvent->pos().toPoint());
    //判断如果点在字体内，则变为选择工具
    /*QPointF pos = mouseEvent->scenePos();
    CGraphicsTextItem *textItem = nullptr;
    m_bIsEditTextFlag = false;
    QList<QGraphicsItem *> items = this->selectedItems();
    foreach (QGraphicsItem *item, items) {
        if (item->type() == TextType) {
            textItem = static_cast<CGraphicsTextItem *>(item);
            m_bIsEditTextFlag = static_cast<CGraphicsTextItem *>(item)->isEditable();
            break;
        }
    }*/


    EDrawToolMode currentMode = CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getCurrentDrawToolMode();

    IDrawTool *pTool = CDrawToolManagerSigleton::GetInstance()->getDrawTool(currentMode);
    /*if (currentMode == text &&  m_bIsEditTextFlag && textItem->rect().contains(pos)) {
        pTool = CDrawToolManagerSigleton::GetInstance()->getDrawTool(selection);
    }*/
    if ( nullptr != pTool) {
        pTool->mousePressEvent(mouseEvent, this);
    }
}

void CDrawScene::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    /*m_bIsEditTextFlag = false;
    QList<QGraphicsItem *> items = this->selectedItems();
    foreach (QGraphicsItem *item, items) {
        if (item->type() == TextType) {
            m_bIsEditTextFlag = static_cast<CGraphicsTextItem *>(item)->isEditable();
            break;
        }
    }*/

    EDrawToolMode currentMode = CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getCurrentDrawToolMode();
    IDrawTool *pTool = CDrawToolManagerSigleton::GetInstance()->getDrawTool(currentMode);
    /*if (currentMode == text && m_bIsEditTextFlag) {
        pTool = CDrawToolManagerSigleton::GetInstance()->getDrawTool(selection);
    }*/
    if ( nullptr != pTool) {
        pTool->mouseMoveEvent(mouseEvent, this);
    }
}

void CDrawScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
//    m_bIsEditTextFlag = false;
//    QList<QGraphicsItem *> items = this->selectedItems();
//    foreach (QGraphicsItem *item, items) {
//        if (item->type() == TextType) {
//            m_bIsEditTextFlag = static_cast<CGraphicsTextItem *>(item)->isEditable();
//            break;
//        }
//    }

    EDrawToolMode currentMode = CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getCurrentDrawToolMode();

    IDrawTool *pTool = CDrawToolManagerSigleton::GetInstance()->getDrawTool(currentMode);
//    if (currentMode == text && m_bIsEditTextFlag) {
//        pTool = CDrawToolManagerSigleton::GetInstance()->getDrawTool(selection);
//    }
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
    EDrawToolMode currentMode = CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getCurrentDrawToolMode();
    setItemDisable(false);
    IDrawTool *pTool = CDrawToolManagerSigleton::GetInstance()->getDrawTool(currentMode);
    if ( nullptr != pTool && cut == pTool->getDrawToolMode()) {
        static_cast<CCutTool *>(pTool)->createCutItem(this);
        emit signalUpdateCutSize();
    }
}

void CDrawScene::quitCutMode()
{
    EDrawToolMode mode = CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getCurrentDrawToolMode();
    if (cut == mode) {
        IDrawTool *pTool = CDrawToolManagerSigleton::GetInstance()->getDrawTool(mode);
        if (nullptr != pTool) {
            static_cast<CCutTool *>(pTool)->deleteCutItem(this);
            setItemDisable(true);
            CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setCurrentDrawToolMode(selection);
            emit signalQuitCutAndChangeToSelect();
        }
    }
}

void CDrawScene::doCutScene()
{
    EDrawToolMode mode = CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getCurrentDrawToolMode();
    if (cut == mode) {
        IDrawTool *pTool = CDrawToolManagerSigleton::GetInstance()->getDrawTool(mode);
        if (nullptr != pTool) {
            QRectF rect = static_cast<CCutTool *>(pTool)->getCutRect();
            if (static_cast<CCutTool *>(pTool)->getModifyFlag()) {
                emit signalSceneCut(rect);
            }
            quitCutMode();
            CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setIsModify(true);
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
    this->clearSelection();
    changeMouseShape(static_cast<EDrawToolMode>(type));
    CDrawScene::GetInstance()->updateBlurItem();


    if (EDrawToolMode(type) == pen) {
        renderSelfToPixmap();
    }
}

void CDrawScene::changeMouseShape(EDrawToolMode type)
{
    switch (type) {
    case selection:
        qApp->setOverrideCursor(QCursor(Qt::ArrowCursor));
        break;
    case importPicture:
        qApp->setOverrideCursor(QCursor(Qt::ArrowCursor));
        break;
    case rectangle:
        qApp->setOverrideCursor(m_rectangleMouse);
        break;
    case ellipse:
        qApp->setOverrideCursor(m_roundMouse);
        break;
    case triangle:
        qApp->setOverrideCursor(m_triangleMouse);
        break;
    case polygonalStar:
        qApp->setOverrideCursor(m_starMouse);
        break;
    case polygon:
        qApp->setOverrideCursor(m_pengatonMouse);
        break;
    case line:
        qApp->setOverrideCursor(m_lineMouse);
        break;
    case pen:
        qApp->setOverrideCursor(m_brushMouse);
        break;
    case text:
        qApp->setOverrideCursor(m_textMouse);
        break;
    case blur:
        qApp->setOverrideCursor(m_blurMouse);
        break;
    case cut:
        qApp->setOverrideCursor(QCursor(Qt::ArrowCursor));
        break;

    default:
        qApp->setOverrideCursor(QCursor(Qt::ArrowCursor));
        break;

    }
}

QPixmap &CDrawScene::scenPixMap()
{
    return m_scenePixMap;
}

void CDrawScene::renderSelfToPixmap()
{
    m_scenePixMap = QPixmap(sceneRect().size().toSize());
    QPainter painterd(&m_scenePixMap);
    painterd.setRenderHint(QPainter::Antialiasing);
    this->render(&painterd);
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
            tmpitem->setFontFamily(CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getTextFont().family());
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
            tmpitem->setFontSize(CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getTextFont().pointSizeF());
        }
    }
}


//if (thisZValue > itemZValue) {
//    retList.push_back(item);
//} else if (thisZValue == itemZValue) {
//    int indexOther = allitems.indexOf(item);
//    if (index > indexOther) {
//        retList.push_back(item);
//    }
//}

void CDrawScene::updateBlurItem(QGraphicsItem *changeItem)
{
    QList<QGraphicsItem *> items = this->items();
    if (changeItem != nullptr) {
        int index = items.indexOf(changeItem);
        qreal zValue = changeItem->zValue();
        foreach (QGraphicsItem *item, items) {
            if (item->type() == BlurType) {
                int blurIndex = items.indexOf(item);
                qreal blurZValue = item->zValue();


                if (blurZValue > zValue) {
                    static_cast<CGraphicsMasicoItem *>(item)->setPixmap();
                }
                //判断在模糊图元下的图元才更新
                else if ((qFuzzyCompare(blurZValue, zValue) && index > blurIndex) || index == -1) {
                    static_cast<CGraphicsMasicoItem *>(item)->setPixmap();
                }
            }
        }
    } else {
        foreach (QGraphicsItem *item, items) {
            if (item->type() == BlurType) {
                static_cast<CGraphicsMasicoItem *>(item)->setPixmap();
            }
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

