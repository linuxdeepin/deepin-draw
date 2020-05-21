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
#include "cgraphicsitemselectedmgr.h"
#include "cgraphicsitemhighlight.h"
#include "drawshape/cpictureitem.h"
#include "frame/cviewmanagement.h"
#include "frame/cgraphicsview.h"
#include "frame/cundocommands.h"
#include "widgets/ctextedit.h"
#include "service/cmanagerattributeservice.h"

#include <QGraphicsSceneMouseEvent>
#include <QDebug>
#include <QRect>
#include <QGraphicsView>
#include <QtMath>
#include <DApplication>
#include <QScrollBar>

DWIDGET_USE_NAMESPACE

CDrawScene::CDrawScene(CGraphicsView *view, const QString &uuid, bool isModified)
    : QGraphicsScene(view)
    , m_drawParam(new CDrawParamSigleton(uuid, isModified))
    , m_bIsEditTextFlag(false)
    , m_drawMouse(QPixmap(":/cursorIcons/draw_mouse.svg"))
    , m_lineMouse(QPixmap(":/cursorIcons/line_mouse.svg"))
    , m_pengatonMouse(QPixmap(":/cursorIcons/pengaton_mouse.svg"))
    , m_rectangleMouse(QPixmap(":/cursorIcons/rectangle_mouse.svg"))
    , m_roundMouse(QPixmap(":/cursorIcons/round_mouse.svg"))
    , m_starMouse(QPixmap(":/cursorIcons/star_mouse.svg"))
    , m_triangleMouse(QPixmap(":/cursorIcons/triangle_mouse.svg"))
    , m_textMouse(QPixmap(":/cursorIcons/text_mouse.svg"), 3, 2)
    , m_brushMouse(QPixmap(":/cursorIcons/brush_mouse.svg"), 7, 26)
    , m_blurMouse(QPixmap(":/cursorIcons/smudge_mouse.png"))
    , m_maxZValue(0)
{
    view->setScene(this);
    initScene();

    connect(this, SIGNAL(itemMoved(QGraphicsItem *, QPointF)),
            view, SLOT(itemMoved(QGraphicsItem *, QPointF)));
    connect(this, SIGNAL(itemAdded(QGraphicsItem *, bool)),
            view, SLOT(itemAdded(QGraphicsItem *, bool)));
    connect(this, SIGNAL(itemRotate(QGraphicsItem *, qreal)),
            view, SLOT(itemRotate(QGraphicsItem *, qreal)));
    connect(this, SIGNAL(itemResize(CGraphicsItem *, CSizeHandleRect::EDirection, QRectF, QPointF, bool, bool )),
            view, SLOT(itemResize(CGraphicsItem *, CSizeHandleRect::EDirection, QRectF, QPointF, bool, bool )));
    connect(this, SIGNAL(itemPropertyChange(CGraphicsItem *, QPen, QBrush, bool, bool)),
            view, SLOT(itemPropertyChange(CGraphicsItem *, QPen, QBrush, bool, bool)));
    connect(this, SIGNAL(itemRectXRediusChange(CGraphicsRectItem *, int, bool)),
            view, SLOT(itemRectXRediusChange(CGraphicsRectItem *, int, bool)));

    connect(this, SIGNAL(itemPolygonPointChange(CGraphicsPolygonItem *, int )),
            view, SLOT(itemPolygonPointChange(CGraphicsPolygonItem *, int )));
    connect(this, SIGNAL(itemPolygonalStarPointChange(CGraphicsPolygonalStarItem *, int, int )),
            view, SLOT(itemPolygonalStarPointChange(CGraphicsPolygonalStarItem *, int, int )));

    connect(this, SIGNAL(itemPenTypeChange(CGraphicsPenItem *, bool, ELineType )),
            view, SLOT(itemPenTypeChange(CGraphicsPenItem *, bool, ELineType)));

    connect(this, SIGNAL(itemBlurChange(CGraphicsMasicoItem *, int, int )),
            view, SLOT(itemBlurChange(CGraphicsMasicoItem *, int, int )));

    connect(this, SIGNAL(itemLineTypeChange(CGraphicsLineItem *, bool, ELineType)),
            view, SLOT(itemLineTypeChange(CGraphicsLineItem *, bool, ELineType)));

    connect(this, SIGNAL(signalQuitCutAndChangeToSelect()),
            view, SLOT(slotRestContextMenuAfterQuitCut()));

    connect(this, SIGNAL(signalSceneCut(QRectF)),
            view, SLOT(itemSceneCut(QRectF)));

}

CDrawScene::~CDrawScene()
{
    delete m_drawParam;
    m_drawParam = nullptr;
}

void CDrawScene::initScene()
{
    m_pGroupItem = new CGraphicsItemSelectedMgr();
    this->addItem(m_pGroupItem);
    m_pGroupItem->setZValue(10000);
    //m_pGroupItem->setFlag(QGraphicsItem::ItemIsSelectable, false);

//    connect(this, &CDrawScene::signalIsModify, this,  [ = ](bool isModdify) {
//        CManageViewSigleton::GetInstance()->updateBlockSystem();
//    });


    m_pHighLightItem = new CGraphicsItemHighLight();
    this->addItem(m_pHighLightItem);
    m_pHighLightItem->setZValue(10000);
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
    case QEvent::GraphicsSceneMouseDoubleClick:
        QGraphicsScene::mouseDoubleClickEvent(mouseEvent);
        break;
    default:
        break;
    }
}

void CDrawScene::drawBackground(QPainter *painter, const QRectF &rect)
{
    //qDebug() << "view count = " << CManageViewSigleton::GetInstance()->viewCount();
    QGraphicsScene::drawBackground(painter, rect);
    if (getDrawParam()->getRenderImage() > 0) {
        if (getDrawParam()->getRenderImage() == 1) {
            painter->fillRect(sceneRect(), Qt::white);
        } else {
            painter->fillRect(sceneRect(), Qt::transparent);
        }
    } else {
//        if (CManageViewSigleton::GetInstance()->getThemeType() == 1) {
//            painter->fillRect(sceneRect(), Qt::white);
//        } else {
//            painter->fillRect(sceneRect(), QColor(55, 55, 55));
//        }
        painter->fillRect(sceneRect(), Qt::white);
    }
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

void CDrawScene::setCursor(const QCursor &cursor)
{
    QList<QGraphicsView *> views  = this->views();
    if ( views.count() > 0 ) {
        QGraphicsView *view = views.first();
        view->setCursor(cursor);
    }
}

void CDrawScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    qDebug() << "---------------CDrawScene::mousePressEvent ---------- ";
    emit signalUpdateColorPanelVisible(mouseEvent->pos().toPoint());

    EDrawToolMode currentMode = getDrawParam()->getCurrentDrawToolMode();

    IDrawTool *pTool = CDrawToolManagerSigleton::GetInstance()->getDrawTool(currentMode);
    if ( nullptr != pTool) {
        pTool->mousePressEvent(mouseEvent, this);
    }
}

void CDrawScene::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if (isBlockMouseMoveEvent())
        return;

    EDrawToolMode currentMode = getDrawParam()->getCurrentDrawToolMode();
    IDrawTool *pTool = CDrawToolManagerSigleton::GetInstance()->getDrawTool(currentMode);
    if ( nullptr != pTool) {
        pTool->mouseMoveEvent(mouseEvent, this);
    }
}

void CDrawScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    qDebug() << "---------------CDrawScene::mouseReleaseEvent ---------- ";

    EDrawToolMode currentMode = getDrawParam()->getCurrentDrawToolMode();

    IDrawTool *pTool = CDrawToolManagerSigleton::GetInstance()->getDrawTool(currentMode);
    IDrawTool *pToolSelect = CDrawToolManagerSigleton::GetInstance()->getDrawTool(EDrawToolMode::selection);
    bool shiftKeyPress = this->getDrawParam()->getShiftKeyStatus();
    if ( nullptr != pTool) {
        pTool->mouseReleaseEvent(mouseEvent, this);
        if (nullptr != pToolSelect && pTool != pToolSelect) {
            //修改bug26618，不是很合理，后期有优化时再作修正
            if (!shiftKeyPress && this->selectedItems().count() == 1) {
                if (this->selectedItems().at(0)->type() > QGraphicsItem::UserType && this->selectedItems().at(0)->type() < MgrType) {
                    pToolSelect->m_noShiftSelectItem = this->selectedItems().at(0);
                }
            }
        }
    }
    CManagerAttributeService::getInstance()->refreshSelectedCommonProperty();
}

void CDrawScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    EDrawToolMode currentMode = getDrawParam()->getCurrentDrawToolMode();

    IDrawTool *pTool = CDrawToolManagerSigleton::GetInstance()->getDrawTool(currentMode);
    if ( nullptr != pTool) {
        pTool->mouseDoubleClickEvent(mouseEvent, this);
    }
}

bool CDrawScene::event(QEvent *event)
{
    QEvent::Type evType = event->type();
    if (evType == QEvent::TouchBegin || evType == QEvent::TouchUpdate || evType == QEvent::TouchEnd) {

        QTouchEvent *touchEvent = dynamic_cast<QTouchEvent *>(event);
        QList<QTouchEvent::TouchPoint> touchPoints = touchEvent->touchPoints();

        EDrawToolMode currentMode = getDrawParam()->getCurrentDrawToolMode();

//        if (currentMode != pen) {
//            return QGraphicsScene::event(event);
//        }


        IDrawTool *pTool = CDrawToolManagerSigleton::GetInstance()->getDrawTool(currentMode);
        if (nullptr != pTool) {
            if (evType != QEvent::TouchUpdate)
                pTool->toolClear();
        }

        foreach ( const QTouchEvent::TouchPoint tp, touchPoints ) {
            IDrawTool::CDrawToolEvent e = IDrawTool::CDrawToolEvent::fromTouchPoint(tp, this);
            switch (tp.state() ) {
            case Qt::TouchPointPressed:
                //表示触碰按下
                QCursor::setPos(e.pos(IDrawTool::CDrawToolEvent::EGlobelPos).toPoint());
                pTool->toolStart(&e);
                break;
            case Qt::TouchPointMoved:
                //触碰移动
                pTool->toolUpdate(&e);
                break;
            case Qt::TouchPointReleased:
                //触碰离开
                pTool->toolFinish(&e);
                break;
            default:
                break;
            }
        }
        if (evType == QEvent::TouchEnd && currentMode == pen) {
            CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setCurrentDrawToolMode(selection);
            emit this->signalChangeToSelect();
        }
        event->accept();
        return true;
    }
    return QGraphicsScene::event(event);
}

void CDrawScene::drawItems(QPainter *painter, int numItems, QGraphicsItem *items[], const QStyleOptionGraphicsItem options[], QWidget *widget)
{
    painter->setClipping(true);
    painter->setClipRect(sceneRect());

    QGraphicsScene::drawItems(painter, numItems, items, options, widget);
}

void CDrawScene::keyReleaseEvent(QKeyEvent *event)
{
    // [0] 解决高亮图元删除后一直显示
    if (!event->isAutoRepeat()) {
        m_pHighLightItem->setVisible(false);
    }
    QGraphicsScene::keyReleaseEvent(event);
}

void CDrawScene::keyPressEvent(QKeyEvent *event)
{
    // [0] 解决高亮图元撤销后一直显示
    if (!event->isAutoRepeat()) {
        m_pHighLightItem->setVisible(false);
    }
    QGraphicsScene::keyPressEvent(event);
}

void CDrawScene::showCutItem()
{
    EDrawToolMode currentMode = getDrawParam()->getCurrentDrawToolMode();
    setItemDisable(false);
    IDrawTool *pTool = CDrawToolManagerSigleton::GetInstance()->getDrawTool(currentMode);
    if ( nullptr != pTool && cut == pTool->getDrawToolMode()) {
        static_cast<CCutTool *>(pTool)->createCutItem(this);
        emit signalUpdateCutSize();
    }
}

void CDrawScene::quitCutMode()
{
    EDrawToolMode mode = getDrawParam()->getCurrentDrawToolMode();
    if (cut == mode) {
        IDrawTool *pTool = CDrawToolManagerSigleton::GetInstance()->getDrawTool(mode);
        if (nullptr != pTool) {
            static_cast<CCutTool *>(pTool)->deleteCutItem(this);
            setItemDisable(true);
            getDrawParam()->setCurrentDrawToolMode(selection);
            emit signalQuitCutAndChangeToSelect();
        }
    }
}

void CDrawScene::doCutScene()
{
    EDrawToolMode mode = getDrawParam()->getCurrentDrawToolMode();
    if (cut == mode) {
        IDrawTool *pTool = CDrawToolManagerSigleton::GetInstance()->getDrawTool(mode);
        if (nullptr != pTool) {
            QRectF rect = static_cast<CCutTool *>(pTool)->getCutRect();
            if (!rect.isNull() && static_cast<CCutTool *>(pTool)->getModifyFlag()) {
                emit signalSceneCut(rect);
            }
            quitCutMode();
            setModify(true);
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

void CDrawScene::doAdjustmentScene(QRectF rect, CGraphicsItem *item)
{
    QUndoCommand *sceneCutCommand = new CSceneCutCommand(this, rect, nullptr, item);
    CManageViewSigleton::GetInstance()->getCurView()->pushUndoStack(sceneCutCommand);
}

void CDrawScene::drawToolChange(int type)
{
    clearMutiSelectedState();
    changeMouseShape(static_cast<EDrawToolMode>(type));
    updateBlurItem();
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
    case blur: {
        // 缩放系数公式： 目的系数 = （1-最大系数）/ （最大值 - 最小值）
        double scanleRate = 0.5 / (500 - 5);
        int blur_width = CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getBlurWidth();
        scanleRate = scanleRate * blur_width + 1.0;

        QPixmap pix = QPixmap(":/cursorIcons/smudge_mouse.png");
        pix = pix.scaled(static_cast<int>(pix.width() * scanleRate), static_cast<int>(pix.height() * scanleRate));
        qApp->setOverrideCursor(pix);
        break;
    }
    case cut:
        qApp->setOverrideCursor(QCursor(Qt::ArrowCursor));
        break;

    default:
        qApp->setOverrideCursor(QCursor(Qt::ArrowCursor));
        break;

    }
}

void CDrawScene::clearMutiSelectedState()
{
    m_pGroupItem->clear();
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
            tmpitem->setFontFamily(getDrawParam()->getTextFont().family());
            tmpitem->setTextFontStyle(getDrawParam()->getTextFontStyle());
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
            tmpitem->setFontSize(getDrawParam()->getTextFont().pointSizeF());
            tmpitem->setTextFontStyle(getDrawParam()->getTextFontStyle());
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
    Q_UNUSED(type);
    QList<QGraphicsItem *> items = this->items();//this->collidingItems();
    //QList<QGraphicsItem *> items = this->collidingItems();
    for (int i = items.size() - 1; i >= 0; i-- ) {
        CGraphicsItem *pItem = dynamic_cast<CGraphicsItem *>(items[i]);
        if (pItem != nullptr) {
            if (pItem->type() == BlurType) {
                static_cast<CGraphicsMasicoItem *>(items[i])->setPixmap();
            }
        }
    }
}

CGraphicsItemSelectedMgr *CDrawScene::getItemsMgr() const
{
    return m_pGroupItem;
}

CGraphicsItemHighLight *CDrawScene::getItemHighLight() const
{
    return m_pHighLightItem;
}

CDrawParamSigleton *CDrawScene::getDrawParam()
{
    return m_drawParam;
}

bool CDrawScene::getModify() const
{
    return m_drawParam->getModify();
}

void CDrawScene::setModify(bool isModify)
{
    //m_drawParam->setModify(isModify);
    emit signalIsModify(isModify);
}

void CDrawScene::setMaxZValue(qreal zValue)
{
    m_pGroupItem->setZValue(zValue + 10000);
    m_pHighLightItem->setZValue(zValue + 10001);
    m_maxZValue = zValue;
}

qreal CDrawScene::getMaxZValue()
{
    return m_maxZValue;
}

void CDrawScene::updateItemsMgr()
{
    int count = m_pGroupItem->getItems().size();
    if (1 == count) {
        m_pGroupItem->hide();
    } else if (count > 1) {
        m_pGroupItem->show();
        clearSelection();
        m_pGroupItem->setSelected(true);
        emit signalAttributeChanged(true, QGraphicsItem::UserType);
    } else {
        emit signalAttributeChanged(true, QGraphicsItem::UserType);
    }

    auto allselectedItems = selectedItems();
    for (int i = allselectedItems.size() - 1; i >= 0; i--) {
        QGraphicsItem *allItem = allselectedItems.at(i);
        if (allItem->type() <= QGraphicsItem::UserType || allItem->type() >= EGraphicUserType::MgrType) {
            allselectedItems.removeAt(i);
            continue;
        }
    }
    if (allselectedItems.size() == 1) {
        allselectedItems.first()->setSelected(true);
        emit signalAttributeChanged(true, allselectedItems.first()->type());
    }
}

void CDrawScene::blockMouseMoveEvent(bool b)
{
    blockMouseMoveEventFlag = b;
}

bool CDrawScene::isBlockMouseMoveEvent()
{
    return blockMouseMoveEventFlag;
}

