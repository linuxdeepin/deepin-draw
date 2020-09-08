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
//#include "frame/cpicturewidget.h"
#include "cgraphicstextitem.h"
#include "ccuttool.h"
#include "cgraphicsmasicoitem.h"
#include "cgraphicstextitem.h"
#include "cgraphicsproxywidget.h"
#include "cgraphicslineitem.h"
#include "cpictureitem.h"
#include "cgraphicsitemselectedmgr.h"
#include "cpictureitem.h"
#include "frame/cviewmanagement.h"
#include "frame/cgraphicsview.h"
#include "widgets/ctextedit.h"
#include "service/cmanagerattributeservice.h"
#include "application.h"
#include "cundoredocommand.h"

#include <QGraphicsSceneMouseEvent>
#include <QDebug>
#include <QRect>
#include <QGraphicsView>
#include <QtMath>
#include <DApplication>
#include <QScrollBar>
#include <QWindow>

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
    , m_textEditCursor(QPixmap(":/theme/light/images/mouse_style/text_mouse.svg"))
{
    view->setScene(this);
    initScene();

    //    connect(this, SIGNAL(itemMoved(QGraphicsItem *, QPointF)),
    //            view, SLOT(itemMoved(QGraphicsItem *, QPointF)));
    connect(this, SIGNAL(itemAdded(QGraphicsItem *, bool)),
            view, SLOT(itemAdded(QGraphicsItem *, bool)));
    //    connect(this, SIGNAL(itemRotate(QGraphicsItem *, qreal)),
    //            view, SLOT(itemRotate(QGraphicsItem *, qreal)));
    //    connect(this, SIGNAL(itemResize(CGraphicsItem *, CSizeHandleRect::EDirection, QRectF, QPointF, bool, bool)),
    //            view, SLOT(itemResize(CGraphicsItem *, CSizeHandleRect::EDirection, QRectF, QPointF, bool, bool)));
    //    connect(this, SIGNAL(itemPropertyChange(CGraphicsItem *, QPen, QBrush, bool, bool)),
    //            view, SLOT(itemPropertyChange(CGraphicsItem *, QPen, QBrush, bool, bool)));
    //    connect(this, SIGNAL(itemRectXRediusChange(CGraphicsRectItem *, int, bool)),
    //            view, SLOT(itemRectXRediusChange(CGraphicsRectItem *, int, bool)));

    //    connect(this, SIGNAL(itemPolygonPointChange(CGraphicsPolygonItem *, int)),
    //            view, SLOT(itemPolygonPointChange(CGraphicsPolygonItem *, int)));
    //    connect(this, SIGNAL(itemPolygonalStarPointChange(CGraphicsPolygonalStarItem *, int, int)),
    //            view, SLOT(itemPolygonalStarPointChange(CGraphicsPolygonalStarItem *, int, int)));

    //    connect(this, SIGNAL(itemPenTypeChange(CGraphicsPenItem *, bool, ELineType)),
    //            view, SLOT(itemPenTypeChange(CGraphicsPenItem *, bool, ELineType)));

    //    connect(this, SIGNAL(itemBlurChange(CGraphicsMasicoItem *, int, int)),
    //            view, SLOT(itemBlurChange(CGraphicsMasicoItem *, int, int)));

    //    connect(this, SIGNAL(itemLineTypeChange(CGraphicsLineItem *, bool, ELineType)),
    //            view, SLOT(itemLineTypeChange(CGraphicsLineItem *, bool, ELineType)));

    connect(this, SIGNAL(signalQuitCutAndChangeToSelect()),
            view, SLOT(slotRestContextMenuAfterQuitCut()));

    connect(this, SIGNAL(signalSceneCut(QRectF)),
            view, SLOT(itemSceneCut(QRectF)));

    connect(this, &CDrawScene::sceneRectChanged, this, [ = ](const QRectF & rect) {
        Q_UNUSED(rect);
        this->updateBlurItem();
    });

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
    m_pGroupItem->setZValue(/*10000*/ INT_MAX);

    //    m_pHighLightItem = new CGraphicsItemHighLight();
    //    this->addItem(m_pHighLightItem);
    //    m_pHighLightItem->setZValue(10000);
}

CGraphicsView *CDrawScene::drawView()
{
    return (views().isEmpty() ? nullptr : qobject_cast<CGraphicsView *>(views().first()));
}

void CDrawScene::mouseEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    switch (mouseEvent->type()) {
    case QEvent::GraphicsSceneMousePress:
        qDebug() << "qt to do SceneMousePress-----";
        QGraphicsScene::mousePressEvent(mouseEvent);
        break;
    case QEvent::GraphicsSceneMouseMove:
        //qDebug() << "qt to do SceneMouseMove----- press = " << mouseEvent->buttons();
        QGraphicsScene::mouseMoveEvent(mouseEvent);
        break;
    case QEvent::GraphicsSceneMouseRelease:
        qDebug() << "qt to do SceneMouseRelease-----";
        QGraphicsScene::mouseReleaseEvent(mouseEvent);
        break;
    case QEvent::GraphicsSceneMouseDoubleClick:
        qDebug() << "qt to do SceneMouseDoubleClick-----";
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
    if (views.count() > 0) {
        QGraphicsView *view = views.first();
        view->setCursor(cursor);
    }
}

void CDrawScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    qDebug() << "CDrawScene::mousePressEvent is touch = " << (mouseEvent->source() == Qt::MouseEventSynthesizedByQt);

    emit signalUpdateColorPanelVisible(mouseEvent->pos().toPoint());

    EDrawToolMode currentMode = getDrawParam()->getCurrentDrawToolMode();

    IDrawTool *pTool = CDrawToolManagerSigleton::GetInstance()->getDrawTool(currentMode);
    if (nullptr != pTool) {
        pTool->mousePressEvent(mouseEvent, this);
    }
}

void CDrawScene::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if (isBlockMouseMoveEvent())
        return;

    EDrawToolMode currentMode = getDrawParam()->getCurrentDrawToolMode();
    IDrawTool *pTool = CDrawToolManagerSigleton::GetInstance()->getDrawTool(currentMode);
    if (nullptr != pTool) {
        pTool->mouseMoveEvent(mouseEvent, this);
    }
}

void CDrawScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    qDebug() << "CDrawScene::mouseReleaseEvent is touch = " << (mouseEvent->source() == Qt::MouseEventSynthesizedByQt);

    EDrawToolMode currentMode = getDrawParam()->getCurrentDrawToolMode();

    IDrawTool *pTool = CDrawToolManagerSigleton::GetInstance()->getDrawTool(currentMode);

    IDrawTool *pToolSelect = CDrawToolManagerSigleton::GetInstance()->getDrawTool(EDrawToolMode::selection);
    bool shiftKeyPress = this->getDrawParam()->getShiftKeyStatus();
    if (nullptr != pTool) {
        if (pTool->isActived() && mouseEvent->button() != Qt::LeftButton) {
            return;
        }
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
    qDebug() << "CDrawScene::mouseDoubleClickEvent flag = " << (mouseEvent->source() == Qt::MouseEventSynthesizedByQt);
    EDrawToolMode currentMode = getDrawParam()->getCurrentDrawToolMode();

    IDrawTool *pTool = CDrawToolManagerSigleton::GetInstance()->getDrawTool(currentMode);
    if (nullptr != pTool) {
        pTool->mouseDoubleClickEvent(mouseEvent, this);
    }
}

void CDrawScene::doLeave()
{
    EDrawToolMode currentMode = getDrawParam()->getCurrentDrawToolMode();

    IDrawTool *pTool = CDrawToolManagerSigleton::GetInstance()->getDrawTool(currentMode);

    if (pTool != nullptr) {
        if (pTool->isActived()) {
            QGraphicsSceneMouseEvent mouseEvent(QEvent::GraphicsSceneMouseRelease);
            mouseEvent.setButton(Qt::LeftButton);
            QPointF pos     =  QCursor::pos();
            QPointF scenPos = pos;
            if (drawView() != nullptr) {
                pos     = drawView()->viewport()->mapFromGlobal(pos.toPoint());
                scenPos = drawView()->mapToScene(scenPos.toPoint());
            }
            mouseEvent.setPos(pos);
            mouseEvent.setScenePos(scenPos);
            //mouseReleaseEvent(&mouseEvent);
            //pTool->interrupt();
        }
    }
}

bool CDrawScene::event(QEvent *event)
{
    QEvent::Type evType = event->type();
    if (evType == QEvent::TouchBegin || evType == QEvent::TouchUpdate || evType == QEvent::TouchEnd) {

        //qDebug() << "CDrawScene:: touch event  evType = " << evType;
        QTouchEvent *touchEvent = dynamic_cast<QTouchEvent *>(event);

        QList<QTouchEvent::TouchPoint> touchPoints = touchEvent->touchPoints();

        EDrawToolMode currentMode = getDrawParam()->getCurrentDrawToolMode();

        IDrawTool *pTool = CDrawToolManagerSigleton::GetInstance()->getDrawTool(currentMode);

        if (nullptr == pTool || (touchPoints.count() > 1 && currentMode == selection)) {
            //是一个手势，那么中断当前的updating操作
            if (pTool != nullptr)
                pTool->interrupt();
            return QGraphicsScene::event(event);
        }

        //解决触屏后鼠标隐藏但还可能鼠标的位置还是高亮的问题
        if (evType == QEvent::TouchBegin) {
            if (drawView() != nullptr) {
                QWindow *pWindow = drawView()->window()->windowHandle();
                QCursor::setPos(pWindow->position() + QPoint(pWindow->size().width(), pWindow->size().height()));
            }
        }

        bool accept = true;
        foreach (const QTouchEvent::TouchPoint tp, touchPoints) {
            IDrawTool::CDrawToolEvent e = IDrawTool::CDrawToolEvent::fromTouchPoint(tp, this, event);
            switch (tp.state()) {
            case Qt::TouchPointPressed:
                //表示触碰按下
                //QCursor::setPos(e.pos(IDrawTool::CDrawToolEvent::EGlobelPos).toPoint());
                pTool->toolDoStart(&e);
                break;
            case Qt::TouchPointMoved:
                //触碰移动
                pTool->toolDoUpdate(&e);
                break;
            case Qt::TouchPointReleased:
                //触碰离开
                pTool->toolDoFinish(&e);
                break;
            default:
                break;
            }
            if (!e.isAccepted()) {
                accept = false;
            }
        }
        event->accept();

        if (evType == QEvent::TouchEnd)
            pTool->clearITE();

        if (accept)
            return true;

    } else if (event->type() == QEvent::Gesture) {
        EDrawToolMode currentMode = getDrawParam()->getCurrentDrawToolMode();

        if (currentMode == selection) {
            return drawView()->gestureEvent(static_cast<QGestureEvent *>(event));
        }
    }
    return QGraphicsScene::event(event);
}

void CDrawScene::drawItems(QPainter *painter, int numItems, QGraphicsItem *items[], const QStyleOptionGraphicsItem options[], QWidget *widget)
{
    painter->setClipping(true);
    painter->setClipRect(sceneRect());

    QGraphicsScene::drawItems(painter, numItems, items, options, widget);
}

void CDrawScene::drawForeground(QPainter *painter, const QRectF &rect)
{
    //禁止绘制时返回
    if (!bDrawForeground)
        return;

    //绘制额外的前景显示，如框选等
    EDrawToolMode currentMode = getDrawParam()->getCurrentDrawToolMode();

    IDrawTool *pTool = CDrawToolManagerSigleton::GetInstance()->getDrawTool(currentMode);

    if (pTool != nullptr) {
        pTool->drawMore(painter, rect, this);

        if (currentMode == selection && !pTool->isActived()) {
            if (!_highlight.isEmpty()) {
                painter->setBrush(Qt::NoBrush);
                DPalette pa = this->palette();
                QBrush selectBrush = pa.brush(QPalette::Active, DPalette:: Highlight);
                QColor selectColor = selectBrush.color();
                QPen p(selectColor);
                p.setWidthF(2.0);
                painter->setPen(p);
                painter->drawPath(_highlight);
            }
        }
    }
}

void CDrawScene::keyReleaseEvent(QKeyEvent *event)
{
    // [0] 解决高亮图元删除后一直显示
    if (!event->isAutoRepeat()) {
        //m_pHighLightItem->setVisible(false);
    }
    QGraphicsScene::keyReleaseEvent(event);
}

void CDrawScene::keyPressEvent(QKeyEvent *event)
{
    // [0] 解决高亮图元撤销后一直显示
    if (!event->isAutoRepeat()) {
        //m_pHighLightItem->setVisible(false);
    }
    QGraphicsScene::keyPressEvent(event);
}

void CDrawScene::refreshLook(const QPointF &pos)
{
    if (drawView() == nullptr)
        return;

    QPainterPath hightlightPath;

    QPointF scenePos = pos;

    if (scenePos.isNull()) {
        QPoint viewPortPos = drawView()->viewport()->mapFromGlobal(QCursor::pos());
        scenePos = drawView()->mapToScene(viewPortPos);
    }

    QList<QGraphicsItem *> items = this->items(scenePos);

    QGraphicsItem *pItem = firstItem(scenePos, items, true, true, false, false);
    CGraphicsItem *pBzItem = dynamic_cast<CGraphicsItem *>(firstItem(scenePos, items,
                                                                     true, true, true, true));

    if (pBzItem != nullptr) {
        hightlightPath = pBzItem->mapToScene(pBzItem->getHighLightPath());
    }

    if (isBussizeHandleNodeItem(pItem)) {
        CSizeHandleRect *pHandle = dynamic_cast<CSizeHandleRect *>(pItem);
        dApp->setApplicationCursor(pHandle->getCursor());
    } else if (pBzItem != nullptr && pBzItem->type() == TextType
               && dynamic_cast<CGraphicsTextItem *>(pBzItem)->isEditable()) {
        dApp->setApplicationCursor(m_textEditCursor);
    } else {
        dApp->setApplicationCursor(Qt::ArrowCursor);
    }

    _highlight = hightlightPath;

    update();
}

void CDrawScene::clearHighlight()
{
    // [41552] 图元移动到属性框下，更改属性图元不需要再高亮
    setHighlightHelper(QPainterPath());
}

void CDrawScene::setHighlightHelper(const QPainterPath &path)
{
    _highlight = path;

    update();
}

QPainterPath CDrawScene::hightLightPath()
{
    return _highlight;
}

void CDrawScene::showCutItem()
{
    EDrawToolMode currentMode = getDrawParam()->getCurrentDrawToolMode();
    setItemDisable(false);
    IDrawTool *pTool = CDrawToolManagerSigleton::GetInstance()->getDrawTool(currentMode);
    if (nullptr != pTool && cut == pTool->getDrawToolMode()) {
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
            QRectF rect = static_cast<CCutTool *>(pTool)->getCutRect(this);
            // 注释代码: [BUG:4553] 在裁剪时，关闭应用时选择保存裁剪，无法保存裁剪内容
            if (!rect.isNull()/* && static_cast<CCutTool *>(pTool)->getModifyFlag()*/) {
                dynamic_cast<CGraphicsView *>(this->views().first())->itemSceneCut(rect);
            }
            quitCutMode();
            this->getDrawParam()->setModify(true);
            setModify(true);
        }
    }

    //更新模糊图元
    QList<QGraphicsItem *> items = this->items(this->sceneRect());

    foreach (QGraphicsItem *item, items) {
        if (item->type() == BlurType) {
            static_cast<CGraphicsMasicoItem *>(item)->updateMasicPixmap();
        }
    }
}

void CDrawScene::doAdjustmentScene(QRectF rect, CGraphicsItem *item)
{
    Q_UNUSED(item)
    //QUndoCommand *sceneCutCommand = new CSceneCutCommand(this, rect, nullptr, item);
    //CManageViewSigleton::GetInstance()->getCurView()->pushUndoStack(sceneCutCommand);

    //if (item->drawScene() != nullptr)
    //    {
    //        this->setSceneRect(rect);
    //    }

    this->setSceneRect(rect.toRect());
}

//void CDrawScene::drawToolChange(int type, bool clearSections)
//{
//    if (clearSections)
//        clearMutiSelectedState();
//    changeMouseShape(static_cast<EDrawToolMode>(type));
//    updateBlurItem();
//}

void CDrawScene::changeMouseShape(EDrawToolMode type)
{
    switch (type) {
    case selection:
        dApp->setApplicationCursor(QCursor(Qt::ArrowCursor));
        break;
    case importPicture:
        dApp->setApplicationCursor(QCursor(Qt::ArrowCursor));
        break;
    case rectangle:
        dApp->setApplicationCursor(m_rectangleMouse);
        break;
    case ellipse:
        dApp->setApplicationCursor(m_roundMouse);
        break;
    case triangle:
        dApp->setApplicationCursor(m_triangleMouse);
        break;
    case polygonalStar:
        dApp->setApplicationCursor(m_starMouse);
        break;
    case polygon:
        dApp->setApplicationCursor(m_pengatonMouse);
        break;
    case line:
        dApp->setApplicationCursor(m_lineMouse);
        break;
    case pen:
        dApp->setApplicationCursor(m_brushMouse);
        break;
    case text:
        dApp->setApplicationCursor(m_textMouse);
        break;
    case blur: {
        // 缩放系数公式： 目的系数 = （1-最大系数）/ （最大值 - 最小值）
        double scanleRate = 0.5 / (500 - 5);
        int blur_width = CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getBlurWidth();
        scanleRate = scanleRate * blur_width + 1.0;

        QPixmap pix = QPixmap(":/cursorIcons/smudge_mouse.png");
        pix = pix.scaled(static_cast<int>(pix.width() * scanleRate), static_cast<int>(pix.height() * scanleRate));
        dApp->setApplicationCursor(pix);
        break;
    }
    case cut:
        dApp->setApplicationCursor(QCursor(Qt::ArrowCursor));
        break;

    default:
        dApp->setApplicationCursor(QCursor(Qt::ArrowCursor));
        break;

    }
}

void CDrawScene::clearMutiSelectedState()
{
    m_pGroupItem->clear();
}

void CDrawScene::setDrawForeground(bool b)
{
    bDrawForeground = b;
}

bool CDrawScene::isDrawedForeground()
{
    return bDrawForeground;
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

//void CDrawScene::textFontFamilyChanged()
//{
//    QList<QGraphicsItem *> items = this->selectedItems();

//    QGraphicsItem *item = nullptr;
//    foreach (item, items) {
//        if (item->type() == TextType) {
//            CGraphicsTextItem *tmpitem = static_cast<CGraphicsTextItem *>(item);
//            tmpitem->setFontFamily(getDrawParam()->getTextFont().family());
//            tmpitem->setTextFontStyle(getDrawParam()->getTextFontStyle());
//        }
//    }
//}

//void CDrawScene::textFontSizeChanged()
//{
//    QList<QGraphicsItem *> items = this->selectedItems();

//    QGraphicsItem *item = nullptr;
//    foreach (item, items) {
//        if (item->type() == TextType) {
//            CGraphicsTextItem *tmpitem = static_cast<CGraphicsTextItem *>(item);
//            tmpitem->setFontSize(getDrawParam()->getTextFont().pointSizeF());
//            tmpitem->setTextFontStyle(getDrawParam()->getTextFontStyle());
//        }
//    }
//}

void CDrawScene::blockUpdateBlurItem(bool b)
{
    blockMscUpdate = b;
}

void CDrawScene::updateBlurItem(QGraphicsItem *changeItem)
{
    Q_UNUSED(changeItem)

    if (blockMscUpdate)
        return;

    QList<QGraphicsItem *> lists = getBzItems();
    foreach (QGraphicsItem *item, lists) {
        if (item->type() == BlurType) {
            static_cast<CGraphicsMasicoItem *>(item)->updateMasicPixmap();
        }
    }
}

void CDrawScene::switchTheme(int type)
{
    Q_UNUSED(type);
    QList<QGraphicsItem *> items = this->items();//this->collidingItems();
    //QList<QGraphicsItem *> items = this->collidingItems();
    for (int i = items.size() - 1; i >= 0; i--) {
        CGraphicsItem *pItem = dynamic_cast<CGraphicsItem *>(items[i]);
        if (pItem != nullptr) {
            if (pItem->type() == BlurType) {
                static_cast<CGraphicsMasicoItem *>(items[i])->updateMasicPixmap();
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

//qreal CDrawScene::totalScalefactor()
//{
//    if (drawView() != nullptr)
//        return drawView()->getScale();
//    return 1.0;
//}

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

bool CDrawScene::isBussizeItem(QGraphicsItem *pItem)
{
    if (pItem == nullptr)
        return false;

    return (pItem->type() >= RectType && pItem->type() <= BlurType);
}

bool CDrawScene::isBussizeHandleNodeItem(QGraphicsItem *pItem)
{
    if (pItem == nullptr)
        return false;
    //CSizeHandleRect的父类QGraphicsSvgItem的类型就是13
    if (pItem->type() == QGraphicsSvgItem::Type) {
        CSizeHandleRect *pHandleItem = dynamic_cast<CSizeHandleRect *>(pItem);
        if (pHandleItem != nullptr) {
            return true;
        }
    } /*else if (pFirstItem->type() == QGraphicsProxyWidget::Type)*/

    return false;
}

bool CDrawScene::isBzAssicaitedItem(QGraphicsItem *pItem)
{
    return (isBussizeItem(pItem) || isBussizeHandleNodeItem(pItem));
}

CGraphicsItem *CDrawScene::getAssociatedBzItem(QGraphicsItem *pItem)
{
    if (pItem == nullptr)
        return nullptr;

    if (isBussizeItem(pItem)) {
        return dynamic_cast<CGraphicsItem *>(pItem);
    }

    if (isBussizeHandleNodeItem(pItem)) {
        CSizeHandleRect *pHandle = dynamic_cast<CSizeHandleRect *>(pItem);
        return dynamic_cast<CGraphicsItem *>(pHandle->parentItem());
    }
    return nullptr;
}

void CDrawScene::clearMrSelection()
{
    clearSelection();
    m_pGroupItem->clear();
}

void CDrawScene::selectItem(QGraphicsItem *pItem, bool onlyBzItem, bool updateAttri, bool updateRect)
{
    if (onlyBzItem && isBussizeItem(pItem)) {
        pItem->setSelected(true);
        m_pGroupItem->add(dynamic_cast<CGraphicsItem *>(pItem), updateAttri, updateRect);
    } else {
        pItem->setSelected(true);
    }
}

void CDrawScene::notSelectItem(QGraphicsItem *pItem, bool updateAttri, bool updateRect)
{
    pItem->setSelected(false);

    if (isBussizeItem(pItem)) {
        m_pGroupItem->remove(dynamic_cast<CGraphicsItem *>(pItem), updateAttri, updateRect);
    }
}

void CDrawScene::selectItemsByRect(const QRectF &rect, bool replace, bool onlyBzItem)
{
    if (replace)
        clearMrSelection();

    QList<QGraphicsItem *> itemlists = this->items(rect);

    for (QGraphicsItem *pItem : itemlists) {
        if (onlyBzItem && isBussizeItem(pItem)) {
            pItem->setSelected(true);
            // 此处可以不用刷新属性,但是文字图元修改为不同样式后导入画板进行框选,显示的属性不对,后续进行改进
            m_pGroupItem->add(dynamic_cast<CGraphicsItem *>(pItem), true, false);
        } else {
            pItem->setSelected(true);
        }
    }
    m_pGroupItem->updateAttributes();
    m_pGroupItem->updateBoundingRect();
}

//void CDrawScene::moveMrItem(const QPointF &prePos, const QPointF &curPos)
//{
//    m_pGroupItem->move(prePos, curPos);
//}

//void CDrawScene::resizeMrItem(CSizeHandleRect::EDirection direction,
//                              const QPointF &prePos,
//                              const QPointF &curPos,
//                              bool keepRadio)
//{
//    m_pGroupItem->newResizeTo(direction, prePos, curPos, keepRadio, false);
//}

QList<QGraphicsItem *> CDrawScene::getBzItems(const QList<QGraphicsItem *> &items)
{
    QList<QGraphicsItem *> lists = items;
    if (lists.isEmpty()) {
        lists = this->items();
    }

    for (int i = 0; i < lists.count();) {
        QGraphicsItem *pItem = lists[i];
        if (!isBussizeItem(pItem)) {
            lists.removeAt(i);
            continue;
        }
        ++i;
    }
    return lists;
}

//降序排列用
static bool zValueSortDES(QGraphicsItem *info1, QGraphicsItem *info2)
{
    //    if(info1->zValue() > info2->zValue())
    //    {
    //        return true;
    //    }
    //    else if(qFuzzyIsNull( info1->zValue() -info2->zValue()))
    //    {
    //        return info1->isObscured()
    //    }
    //    return false;
    if (qFuzzyIsNull(info1->zValue() - info2->zValue())) {
        qDebug() << "same ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ = " << info1->zValue();
    }
    return info1->zValue() >= info2->zValue();
}
//升序排列用
static bool zValueSortASC(QGraphicsItem *info1, QGraphicsItem *info2)
{
    return info1->zValue() <= info2->zValue();
}

void CDrawScene::sortZ(QList<QGraphicsItem *> &list, CDrawScene::ESortItemTp tp)
{
    auto f = (tp == EAesSort ? zValueSortASC : zValueSortDES);

    qSort(list.begin(), list.end(), f);
}

QList<QGraphicsItem *> CDrawScene::returnSortZItems(const QList<QGraphicsItem *> &list, CDrawScene::ESortItemTp tp)
{
    QList<QGraphicsItem *> sorts = list;
    sortZ(sorts, tp);
    return sorts;
}

CGraphicsItem *CDrawScene::topBzItem(const QPointF &pos, bool penalgor, int IncW)
{
    return dynamic_cast<CGraphicsItem *>(firstItem(pos, QList<QGraphicsItem *>(),
                                                   true, penalgor, true, true, true, IncW));
}

CGraphicsItem *CDrawScene::firstBzItem(const QList<QGraphicsItem *> &items, bool haveDesSorted)
{
    auto fFindBzItem = [ = ](const QList<QGraphicsItem *> &_list) {
        CGraphicsItem *pResult = nullptr;
        for (int i = 0; i < _list.count(); ++i) {
            QGraphicsItem *it = _list[i];
            if (isBussizeItem(it)) {
                pResult = dynamic_cast<CGraphicsItem *>(it);
                break;
            }
        }
        return pResult;
    };

    if (!haveDesSorted) {
        const QList<QGraphicsItem *> &list = returnSortZItems(items);
        return fFindBzItem(list);
    }
    return fFindBzItem(items);
}

QGraphicsItem *CDrawScene::firstItem(const QPointF &pos,
                                     const QList<QGraphicsItem *> &itemsCus,
                                     bool isListDesSorted,
                                     bool penalgor,
                                     bool isBzItem,
                                     bool seeNodeAsBzItem,
                                     bool filterMrAndHightLight, int incW)
{
    QList<QGraphicsItem *> items = itemsCus;

    if (itemsCus.isEmpty()) {
        if (incW == 0) {
            items = this->items(pos);
        } else {
            items = this->items(QRectF(pos - QPoint(incW, incW), 2 * QSize(incW, incW)));
        }
    }

    //先去掉多选图元和高亮图元
    if (filterMrAndHightLight) {
        for (int i = 0; i < items.count();) {
            QGraphicsItem *pItem = items[i];
            bool isAsscMgr = false;
            if (pItem->type() == MgrType || pItem->type() == CGraphicsProxyWidget::Type) {
                //isAsscMgr = true;
                items.removeAt(i);
                continue;
            } else if (isBussizeHandleNodeItem(pItem)) {
                if (seeNodeAsBzItem) {
                    pItem = getAssociatedBzItem(pItem);
                    isAsscMgr = (pItem != nullptr && pItem->type() == MgrType);
                }
            }

            if (isAsscMgr) {
                CGraphicsItemSelectedMgr *pSelctMrItem = dynamic_cast<CGraphicsItemSelectedMgr *>(pItem);
                QList<CGraphicsItem *> selects = pSelctMrItem->getItems();
                if (selects.count() == 1) {
                    items.replace(i, selects.first());
                } else {
                    items.removeAt(i);
                    continue;
                }
            }

            ++i;
        }
    }

    //如果list没有排好序那么要进行先排序
    if (!isListDesSorted) {
        sortZ(items);
    }

    if (items.isEmpty())
        return nullptr;

    if (penalgor) {
        QGraphicsItem *pResultItem = nullptr;
        QList<CGraphicsItem *> allPenetrable;
        for (int i = 0; i < items.count(); ++i) {
            QGraphicsItem *pItem = items[i];
            if (isBussizeItem(pItem)) {
                CGraphicsItem *pBzItem = dynamic_cast<CGraphicsItem *>(pItem);
                if (!pBzItem->isPosPenetrable(pBzItem->mapFromScene(pos))) {
                    //在该位置不透明,判定完成
                    //                    pResultItem = pPreTransBzItem == nullptr ? pBzItem : pPreTransBzItem;
                    pResultItem = pBzItem;
                    break;
                } else {
                    allPenetrable.append(pBzItem);
                }
            } else {
                //非业务图元无认识不透明的 ，那么就证明找到了，判定完成
                pResultItem = pItem;

                //将非业务图元的节点图元看作业务图元时的情况
                if (isBussizeHandleNodeItem(pItem)) {
                    if (seeNodeAsBzItem) {
                        CSizeHandleRect *pHandelItem = dynamic_cast<CSizeHandleRect *>(pItem);
                        CGraphicsItem *pBzItem = dynamic_cast<CGraphicsItem *>(pHandelItem->parentItem());
                        pResultItem = pBzItem;
                    }
                }

                //必须返回业务图元的情况
                if (isBzItem && !isBussizeItem(pResultItem)) {
                    pResultItem = nullptr;
                }

                break;
            }
        }
        if (pResultItem == nullptr) {
            if (!allPenetrable.isEmpty()) {
                QRectF ins = allPenetrable.first()->mapToScene(allPenetrable.first()->boundingRect()).boundingRect();
                for (auto it : allPenetrable) {
                    QRectF rect = it->mapToScene(it->boundingRect()).boundingRect();
                    ins = ins.intersected(rect);
                }
                qreal maxRadio = 0;
                for (auto it : allPenetrable) {
                    QRectF rect = it->mapToScene(it->boundingRect()).boundingRect();
                    qreal radio = (ins.width() * ins.height()) / (rect.width() * rect.height());
                    if (radio > maxRadio) {
                        maxRadio = radio;
                        pResultItem = it;
                    }
                }
            }
        }
        return pResultItem;
    }
    QGraphicsItem *pRetItem = nullptr;
    if (isBzItem) {
        for (auto item : items) {
            if (isBussizeItem(item)) {
                pRetItem = item;
                break;
            } else {
                if (seeNodeAsBzItem && isBussizeHandleNodeItem(item)) {
                    pRetItem = getAssociatedBzItem(item);
                    break;
                }
            }
        }
    } else {
        pRetItem = items.isEmpty() ? nullptr : items.first();
    }
    return pRetItem;
}

QGraphicsItem *CDrawScene::firstNotMrItem(const QList<QGraphicsItem *> items)
{
    for (auto it : items) {
        if (it->type() == MgrType)
            continue;
        return it;
    }
    return nullptr;
}

void CDrawScene::moveItems(const QList<QGraphicsItem *> &itemlist, const QPointF &move)
{
    for (QGraphicsItem *pItem : itemlist) {
        pItem->moveBy(move.x(), move.y());
    }
}

//void CDrawScene::rotatBzItem(CGraphicsItem *pBzItem, qreal angle)
//{
//    if (pBzItem == nullptr)
//        return;

//    pBzItem->rotatAngle(angle);
//}

void CDrawScene::setMaxZValue(qreal zValue)
{
    m_pGroupItem->setZValue(zValue + 10000);
    //m_pHighLightItem->setZValue(zValue + 10001);
    m_maxZValue = zValue;
}

qreal CDrawScene::getMaxZValue()
{
    return m_maxZValue;
}

//void CDrawScene::updateItemsMgr()
//{
//    int count = m_pGroupItem->getItems().size();
//    if (1 == count) {
//        m_pGroupItem->hide();
//    } else if (count > 1) {
//        m_pGroupItem->show();
//        clearSelection();
//        m_pGroupItem->setSelected(true);
//        emit signalAttributeChanged(true, QGraphicsItem::UserType);
//    } else {
//        emit signalAttributeChanged(true, QGraphicsItem::UserType);
//    }

//    auto allselectedItems = selectedItems();
//    for (int i = allselectedItems.size() - 1; i >= 0; i--) {
//        QGraphicsItem *allItem = allselectedItems.at(i);
//        if (allItem->type() <= QGraphicsItem::UserType || allItem->type() >= EGraphicUserType::MgrType) {
//            allselectedItems.removeAt(i);
//            continue;
//        }
//    }
//    if (allselectedItems.size() == 1) {
//        allselectedItems.first()->setSelected(true);
//        emit signalAttributeChanged(true, allselectedItems.first()->type());
//    }
//}

void CDrawScene::blockMouseMoveEvent(bool b)
{
    blockMouseMoveEventFlag = b;
}

bool CDrawScene::isBlockMouseMoveEvent()
{
    return blockMouseMoveEventFlag;
}

void CDrawScene::recordItemsInfoToCmd(const QList<CGraphicsItem *> &items, bool isUndo)
{
    for (int i = 0; i < items.size(); ++i) {
        CGraphicsItem *pItem = items[i];

        QList<QVariant> vars;
        vars << reinterpret_cast<long long>(pItem);
        QVariant varInfo;
        varInfo.setValue(pItem->getGraphicsUnit(false));
        vars << varInfo;

        if (isUndo) {
            CUndoRedoCommand::recordUndoCommand(CUndoRedoCommand::EItemChangedCmd,
                                                CItemUndoRedoCommand::EAllChanged, vars, i == 0);
        } else {
            CUndoRedoCommand::recordRedoCommand(CUndoRedoCommand::EItemChangedCmd,
                                                CItemUndoRedoCommand::EAllChanged, vars);
        }
    }
}

void CDrawScene::finishRecord(bool doRedoCmd)
{
    CUndoRedoCommand::finishRecord(doRedoCmd);
}
