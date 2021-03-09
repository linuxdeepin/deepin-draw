/*
 * Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
 *
 * Author:     Ji XiangLong <jixianglong@uniontech.com>
 *
 * Maintainer: WangYu <wangyu@uniontech.com>
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
#include "application.h"
#include "cundoredocommand.h"
#include "clefttoolbar.h"
#include "cgraphicsellipseitem.h"
#include "cgraphicstriangleitem.h"

#include <QGraphicsSceneMouseEvent>
#include <QDebug>
#include <QRect>
#include <QGraphicsView>
#include <QPainterPath>
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
    , m_pSelGroupItem(nullptr)
    , m_textEditCursor(QPixmap(":/theme/light/images/mouse_style/text_mouse.svg"))
{
    view->setScene(this);

    resetScene();

    connect(this, SIGNAL(signalQuitCutAndChangeToSelect()),
            view, SLOT(slotRestContextMenuAfterQuitCut()));

    connect(this, SIGNAL(signalSceneCut(QRectF)),
            view, SLOT(itemSceneCut(QRectF)));

    initCursor();
}

CDrawScene::~CDrawScene()
{
    delete m_drawParam;
    m_drawParam = nullptr;
}

void CDrawScene::resetScene()
{
    clear();
    m_pSelGroupItem = nullptr;
    if (m_pSelGroupItem == nullptr) {
        m_pSelGroupItem = new CGraphicsItemGroup(CGraphicsItemGroup::ESelectGroup);
        this->addItem(m_pSelGroupItem);
        m_pSelGroupItem->setZValue(INT_MAX);

        //绑定选中的图元变化信号进行刷新工具状态
        if (drawApp->leftToolBar() != nullptr)
            drawApp->leftToolBar()->updateToolBtnState();

        connect(m_pSelGroupItem, &CGraphicsItemGroup::childrenChanged, this, [ = ](const QList<CGraphicsItem * > &children) {
            Q_UNUSED(children)
            CGraphicsItemGroup *pSeleGp = m_pSelGroupItem;
            if (pSeleGp == nullptr || pSeleGp->drawScene() == nullptr)
                return;
            if (pSeleGp->curView() != CManageViewSigleton::GetInstance()->getCurView())
                return;

            if (drawApp->leftToolBar() != nullptr)
                drawApp->leftToolBar()->updateToolBtnState();
        });
    }
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
            QMetaObject::invokeMethod(this, [ = ]() {
                pTool->interrupt();
            }, Qt::QueuedConnection);
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
            DToolButton *pBtn = drawApp->leftToolBar()->toolButton(currentMode);
            if (pBtn != nullptr) {
                pBtn->setAttribute(Qt::WA_UnderMouse, false);
                pBtn->update();
            }
        }

        bool accept = true;
        foreach (const QTouchEvent::TouchPoint tp, touchPoints) {
            IDrawTool::CDrawToolEvent e = IDrawTool::CDrawToolEvent::fromTouchPoint(tp, this, event);
            switch (tp.state()) {
            case Qt::TouchPointPressed:
                //表示触碰按下
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
        if (currentMode == selection && !pTool->isActived() && drawView()->activeProxWidget() == nullptr) {
            if (!_highlight.isEmpty()) {
                painter->setBrush(Qt::NoBrush);
                QBrush selectBrush = drawApp->systemThemeColor();
                QColor selectColor = selectBrush.color();
                QPen p(selectColor);
                p.setWidthF(2.0);
                painter->setPen(p);
                painter->drawPath(_highlight);
            }
        }
    }

    QGraphicsScene::drawForeground(painter, rect);
}

void CDrawScene::refreshLook(const QPointF &pos)
{
    if (drawView() == nullptr)
        return;


    if (getDrawParam()->getCurrentDrawToolMode() != selection) {
        clearHighlight();
        return;
    }

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
    if (pBzItem != nullptr && pBzItem->type() != BlurType) {
        hightlightPath = pBzItem->mapToScene(pBzItem->getHighLightPath());
    }

    if (isBussizeHandleNodeItem(pItem)) {
        CSizeHandleRect *pHandle = dynamic_cast<CSizeHandleRect *>(pItem);
        drawApp->setApplicationCursor(pHandle->getCursor());
    } else if (pBzItem != nullptr && pBzItem->type() == TextType
               && dynamic_cast<CGraphicsTextItem *>(pBzItem)->isEditState()) {
        drawApp->setApplicationCursor(m_textEditCursor);
    } else {
        drawApp->setApplicationCursor(Qt::ArrowCursor);
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

//QPainterPath CDrawScene::hightLightPath()
//{
//    return _highlight;
//}

CDrawScene::CGroupBzItemsTree CDrawScene::getGroupTree(CGraphicsItemGroup *pGroup)
{
    CGroupBzItemsTree info;

    //为nullptr表示收集当前场景下的组合情况
    if (pGroup == nullptr) {
        for (auto pGroup : m_pGroups) {
            if (pGroup->isTopBzGroup()) {
                info.childGroups.append(getGroupTree(pGroup));
            }
        }
        info.setGroupType(CGraphicsItemGroup::EVirRootGroup);
        return info;
    }

    info.pGroup = pGroup;
    info.setGroupType(pGroup->groupType());
    CGraphicsUnit tempGroupData = pGroup->getGraphicsUnit(EDuplicate);
    CGraphicsUnit::deepCopy(info.data, tempGroupData);
    tempGroupData.release();

    QList<CGraphicsItem *> items = pGroup->items();
    for (auto it : items) {
        if (isNormalGroupItem(it)) {
            info.childGroups.append(getGroupTree(static_cast<CGraphicsItemGroup *>(it)));
        } else {
            info.bzItems.append(it);
        }
    }
    return info;
}

CGroupBzItemsTreeInfo CDrawScene::getGroupTreeInfo(CGraphicsItemGroup *pGroup, EDataReason reson)
{
    CGroupBzItemsTreeInfo info;

    //为nullptr表示收集当前场景下的组合情况
    if (pGroup == nullptr) {
        for (auto pGroup : m_pGroups) {
            if (pGroup->isTopBzGroup()) {
                info.childGroups.append(getGroupTreeInfo(pGroup));
            }
        }
        if (reson == ESaveToDDf) {
            auto bzItems = getBzItems();
            for (auto p : bzItems) {
                if (p->bzGroup(true) == nullptr) {
                    info.bzItems.append(p->getGraphicsUnit(reson));
                }
            }
        }
        info.setGroupType(CGraphicsItemGroup::EVirRootGroup);
        info.data.head.dataType = MgrType;
        return info;
    }
//    if (pGroup == nullptr)
//        return info;

    CGraphicsUnit tempGroupData = pGroup->getGraphicsUnit(reson);
    CGraphicsUnit::deepCopy(info.data, tempGroupData);
    tempGroupData.release();

    //只允许有一个选择组合图元
    if (info.groupType() == CGraphicsItemGroup::ESelectGroup) {
        info.setGroupType(CGraphicsItemGroup::EVirRootGroup);
    }

    //按照升序进行保存,这样在被加载回来的时候z值可以从低到高正确
    auto groups  = pGroup->getGroups();
    auto fSortGroup = [ = ](CGraphicsItemGroup * gp1, CGraphicsItemGroup * gp2) {
        return gp1->getMinZ() <= gp2->getMinZ();
    };
    qSort(groups.begin(), groups.end(), fSortGroup);

    auto bzItems = pGroup->getBzItems();
    auto fSortItems = [ = ](CGraphicsItem * it1, CGraphicsItem * it2) {
        return it1->zValue() <= it2->zValue();
    };
    qSort(bzItems.begin(), bzItems.end(), fSortItems);

    for (auto gp : groups) {
        info.childGroups.append(getGroupTreeInfo(gp));
    }
    for (auto it : bzItems) {
        info.bzItems.append(it->getGraphicsUnit(reson));
    }
    return info;
}

CGraphicsItemGroup *CDrawScene::loadGroupTree(const CDrawScene::CGroupBzItemsTree &info)
{
    //如果是顶层根管理类型(顶层) 那么先清空当前场景内的组合情况
    if (info.groupType() == CGraphicsItemGroup::EVirRootGroup)
        destoryAllGroup();


    CGraphicsItemGroup *pGroup = (info.pGroup == nullptr ? nullptr : static_cast<CGraphicsItemGroup *>(info.pGroup));

    QList<CGraphicsItem *> items = info.bzItems;

    for (auto it : info.childGroups) {
        CGraphicsItemGroup *pChildGroup = loadGroupTree(it);
        if (pChildGroup != nullptr)
            items.append(pChildGroup);
    }

    if (!items.isEmpty()) {
        if (pGroup == nullptr)
            pGroup = creatGroup(items, info.groupType());
        else {
            for (auto p : items) {
                pGroup->add(p, false, false);
            }
            pGroup->setRecursiveScene(this);
            pGroup->updateBoundingRect();
            if (!m_pGroups.contains(pGroup))
                m_pGroups.append(pGroup);
            selectItem(pGroup);
        }
        pGroup->loadGraphicsUnit(info.data);
    }
    return pGroup;
}

CGraphicsItemGroup *CDrawScene::loadGroupTreeInfo(const CGroupBzItemsTreeInfo &info, bool notClear)
{
    //如果是非常规组合 那么先清空当前场景内的组合情况
    if (info.groupType() != CGraphicsItemGroup::ENormalGroup && !notClear)
        destoryAllGroup();

    CGraphicsItemGroup *pGroup = nullptr;

    QList<CGraphicsUnit> utItems = info.bzItems;
    QList<CGraphicsItem *> items;
    for (auto ut : utItems) {
        CGraphicsItem *pItem = CGraphicsItem::creatItemInstance(ut.head.dataType, ut);
        items.append(pItem);
    }

    for (auto it : info.childGroups) {
        CGraphicsItemGroup *pChildGroup = loadGroupTreeInfo(it);
        if (pChildGroup != nullptr)
            items.append(pChildGroup);
    }

    if (!items.isEmpty()) {
        pGroup = creatGroup(items, info.groupType());
        pGroup->loadGraphicsUnit(info.data);
    }

    return pGroup;
}

void CDrawScene::releaseBzItemsTreeInfo(CGroupBzItemsTreeInfo info)
{
    for (int i = 0; i < info.childGroups.size(); ++i) {
        CGroupBzItemsTreeInfo &childInfo = info.childGroups[i];
        releaseBzItemsTreeInfo(childInfo);
    }
    for (int i = 0; i < info.bzItems.size(); ++i) {
        info.bzItems[i].release();
    }
}

void assignmentZ(const QList<CGraphicsItem *> &items, qreal &beginZ, CDrawScene::ESortItemTp listSortedTp)
{
    int beginIndex = listSortedTp == CDrawScene::EDesSort ? items.size() - 1 : 0;
    int endIndex   = listSortedTp == CDrawScene::EDesSort ? 0 : items.size();
    int inc = listSortedTp == CDrawScene::EDesSort ? -1 : 1;
    for (int i = beginIndex; listSortedTp == CDrawScene::EDesSort ? i >= endIndex : i < endIndex; i += inc) {
        CGraphicsItem *p = items.at(i);
        if (p->isBzItem()) {
            p->setZValue(beginZ);
            ++beginZ;
        } else if (p->isBzGroup()) {
            auto pGroup = static_cast<CGraphicsItemGroup *>(p);
            assignmentZ(CDrawScene::returnSortZItems(pGroup->items(), listSortedTp), beginZ, listSortedTp);
        }
    }
}

void CDrawScene::moveItemsZDown(const QList<CGraphicsItem *> &items, int step)
{
    //1.首先判断图元们是否是一个组合的,要移动的图元需要是处于一个组合中的(或者都不处于组合)
    CGraphicsItemGroup *pSameGroup = getSameGroup(items, false, false, true);
    if (pSameGroup == nullptr)
        return;

    //2.获取会受到影响的图元(按照z降序排列,即从z值大到小排序)
    qreal minZ = 0;
    QList<CGraphicsItem *> invokedItems;
    if (pSameGroup->groupType() == CGraphicsItemGroup::ENormalGroup) {
        invokedItems = pSameGroup->items();
        sortZ(invokedItems, EDesSort);
        minZ = invokedItems.last()->zValue();
    } else {
        invokedItems = getRootItems(EDesSort);
        minZ = 0;
    }
    //3.进行排序
    bool toLimitFirst = true;
    //按照z值从低到高进行遍历,遇到想移动的图元见按照步数在invokedItems链表向后移动
    for (int i = invokedItems.size() - 1; i >= 0; --i) {
        CGraphicsItem *p = invokedItems.at(i);
        if (items.contains(p)) {
            //向下移动step个图层
            int index = (i + step + 1);
            if (step == -1) {
                if (toLimitFirst) {
                    index = invokedItems.size();
                    toLimitFirst = false;
                } else {
                    index = invokedItems.size() - 1;
                }
            }
            //边界检查
            if (index > invokedItems.size()) {
                index = invokedItems.size();
            }
            //先插入再删除以实现将当前的图元移动到正确的位置
            invokedItems.insert(index, p);
            invokedItems.removeAt(i);
        }
    }

    //4.循环遍历给于新的z值
    assignmentZ(invokedItems, minZ, EDesSort);

    if (pSameGroup->groupType() == CGraphicsItemGroup::EVirRootGroup) {
        //delete pSameGroup;
        pSameGroup->clear();
        pSameGroup->deleteLater();
    }
}

void CDrawScene::moveItemsZUp(const QList<CGraphicsItem *> &items, int step)
{
    //1.首先判断图元们是否是一个组合的,要移动的图元需要是处于一个组合中的(或者都不处于组合)
    CGraphicsItemGroup *pSameGroup = getSameGroup(items, false, false, true);
    if (pSameGroup == nullptr) {
        return;
    }

    //2.获取会受到影响的图元(按照z升序排列,即从z值小到大排序)
    qreal minZ = 0;
    QList<CGraphicsItem *> invokedItems;
    if (pSameGroup->groupType() == CGraphicsItemGroup::ENormalGroup) {
        invokedItems = pSameGroup->items();
        sortZ(invokedItems, EAesSort);
        minZ = invokedItems.first()->zValue();
    } else {
        invokedItems = getRootItems(EAesSort);
        minZ = 0;
    }
    //3.进行排序
    bool toLimitFirst = true;
    //按照z值从高到低进行遍历,遇到想移动的图元见按照步数在invokedItems链表向后动
    for (int i = invokedItems.size() - 1; i >= 0; --i) {
        CGraphicsItem *p = invokedItems.at(i);
        if (items.contains(p)) {
            int index = (i + step + 1);
            if (step == -1) {
                if (toLimitFirst) {
                    index = invokedItems.size();
                    toLimitFirst = false;
                } else {
                    index = invokedItems.size() - 1;
                }
            }
            //边界检查
            if (index > invokedItems.size()) {
                index = invokedItems.size();
            }
            //先插入再删除以实现将当前的图元移动到正确的位置
            invokedItems.insert(index, p);
            invokedItems.removeAt(i);
        }
    }

    //4.循环遍历给于新的z值
    assignmentZ(invokedItems, minZ, EAesSort);

    if (pSameGroup->groupType() == CGraphicsItemGroup::EVirRootGroup) {
        //delete pSameGroup;
        pSameGroup->clear();
        pSameGroup->deleteLater();
    }
}

void CDrawScene::sortZBaseOneBzItem(const QList<CGraphicsItem *> &items, CGraphicsItem *pBaseItem)
{
    //0.必须保证基准图元时在被操作的图元中的
    assert(items.contains(pBaseItem));

    //场景下所有的基本业务图元
    auto scenBzItems = this->getBzItems();
    int  tempZ = scenBzItems.size() - 1;

    QList<QList<CGraphicsItem *>> tempListLists;

    QList<CGraphicsItem *> temp;
    int baszZIndex = 0;
    QList<int> groupListIndexList;

    bool nextListIsBase = false;

    bool preIemInGroup = false;
    bool frst = true;
    for (auto p : scenBzItems) {
        CGraphicsItem *pBz = static_cast<CGraphicsItem *>(p);
        bool isInGroup = items.contains(pBz);
        if (isInGroup == preIemInGroup || frst) {
        } else {
            if (nextListIsBase) {
                baszZIndex = tempListLists.size();
                nextListIsBase = false;
            }
            if (preIemInGroup) {
                groupListIndexList.append(tempListLists.size());
            }
            tempListLists.append(temp);
            temp.clear();
        }
        temp.append(pBz);
        if (isInGroup && pBz == pBaseItem) {
            nextListIsBase = true;
        }
        preIemInGroup = isInGroup;
        frst = false;
    }

    //再判断一次,进行边界收尾
    if (nextListIsBase) {
        baszZIndex = tempListLists.size();
    }
    if (preIemInGroup) {
        groupListIndexList.append(tempListLists.size());
    }
    tempListLists.append(temp);
    temp.clear();

    for (int i = 0; i < tempListLists.size(); ++i) {
        if (i == baszZIndex) {
            for (auto index : groupListIndexList) {
                auto list = tempListLists.at(index);
                for (int j = 0; j < list.size(); ++j) {
                    list.at(j)->setZValue(tempZ);
                    --tempZ;
                }
            }

        } else {
            if (!groupListIndexList.contains(i)) {
                auto list = tempListLists.at(i);
                for (int j = 0; j < list.size(); ++j) {
                    list.at(j)->setZValue(tempZ);
                    --tempZ;
                }
            }
        }
    }

    //弄脏z值,获取到组合的正确z值
    for (auto pG : m_pGroups) {
        pG->markZDirty();
    }
#ifdef QT_DEBUG
    //打印测试
    {
        qDebug() << "begin print z:";
        auto itemsggg = this->getBzItems();
        qDebug() << "scenBzItems count = " << itemsggg.count();
        for (auto p : itemsggg) {
            qDebug() << "-------z = " << p->zValue();
        }
        qDebug() << "end   print z:";
    }
#endif
}

void CDrawScene::initCursor()
{
    m_textEditCursor = QCursor(Qt::IBeamCursor);
    m_textMouse = QCursor(Qt::IBeamCursor);
    qreal radio = qApp->devicePixelRatio();
    if (qFuzzyIsNull(radio - 1.0))
        return;
    //return;

    QStringList cursorPixSvgSrcs;

    cursorPixSvgSrcs << ":/cursorIcons/draw_mouse.svg" << ":/cursorIcons/line_mouse.svg" << ":/cursorIcons/pengaton_mouse.svg"
                     << ":/cursorIcons/rectangle_mouse.svg" << ":/cursorIcons/round_mouse.svg" << ":/cursorIcons/star_mouse.svg"
                     << ":/cursorIcons/triangle_mouse.svg" << ":/cursorIcons/text_mouse.svg" << ":/cursorIcons/brush_mouse.svg"
                     << ":/cursorIcons/smudge_mouse.png" /*<< ":/theme/light/images/mouse_style/text_mouse.svg"*/;
    QList<QCursor *> memberCursors;
    memberCursors << &m_drawMouse << &m_lineMouse << &m_pengatonMouse
                  << &m_rectangleMouse << &m_roundMouse << &m_starMouse
                  << &m_triangleMouse << &m_textMouse << &m_brushMouse
                  << &m_blurMouse;

    QList<QPoint> offset;
    QPoint defualtOffset(0, 0);
    offset << defualtOffset << defualtOffset << defualtOffset
           << defualtOffset << defualtOffset << defualtOffset
           << defualtOffset << defualtOffset << QPoint(-7, 8)
           << defualtOffset;


    QSvgRenderer svgRender;
    for (int i = 0; i < cursorPixSvgSrcs.size(); ++i) {
        auto srcPath = cursorPixSvgSrcs.at(i);
        auto pCursor = memberCursors.at(i);
        QPointF extrOffset = QPoint(qRound(offset.at(i).x() * radio), qRound(offset.at(i).y() * radio));
        if (svgRender.load(srcPath)) {
            QPixmap pix(QSize(32, 32)*radio);
            pix.fill(QColor(0, 0, 0, 0));
            QPainter painter(&pix);
            svgRender.render(&painter, QRect(QPoint(0, 0), pix.size()));

            *pCursor = QCursor(pix, pix.width() / 2 + extrOffset.x(), pix.height() / 2 + extrOffset.y());
        } else {
            QPixmap pixsource(srcPath);
            if (!pixsource.isNull()) {
                QPixmap pix(QSize(32, 32)*radio);
                pix.fill(QColor(0, 0, 0, 0));
                QPainter painter(&pix);
                painter.setRenderHint(QPainter::SmoothPixmapTransform);
                painter.drawPixmap(QRect(QPoint(0, 0), pix.size()), pixsource);
                //pix = pix.scaled(QSize(32, 32) * radio, Qt::KeepAspectRatio, Qt::SmoothTransformation);
                *pCursor = QCursor(pix, pix.width() / 2 + extrOffset.x(), pix.height() / 2 + extrOffset.y());
            }
        }
    }
}

void CDrawScene::showCutItem()
{
    EDrawToolMode currentMode = getDrawParam()->getCurrentDrawToolMode();
    setItemsActive(false);
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
            setItemsActive(true);
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

#ifdef SINGLEMASIC
    foreach (QGraphicsItem *item, items) {
        if (item->type() == BlurType) {
            static_cast<CGraphicsMasicoItem *>(item)->updateMasicPixmap();
        }
    }
#endif
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
    this->setSceneRect(rect/*.toRect()*/);
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
        drawApp->setApplicationCursor(QCursor(Qt::ArrowCursor));
        break;
    case importPicture:
        drawApp->setApplicationCursor(QCursor(Qt::ArrowCursor));
        break;
    case rectangle:
        drawApp->setApplicationCursor(m_rectangleMouse);
        break;
    case ellipse:
        drawApp->setApplicationCursor(m_roundMouse);
        break;
    case triangle:
        drawApp->setApplicationCursor(m_triangleMouse);
        break;
    case polygonalStar:
        drawApp->setApplicationCursor(m_starMouse);
        break;
    case polygon:
        drawApp->setApplicationCursor(m_pengatonMouse);
        break;
    case line:
        drawApp->setApplicationCursor(m_lineMouse);
        break;
    case pen:
        drawApp->setApplicationCursor(m_brushMouse);
        break;
    case text:
        //drawApp->setApplicationCursor(m_textMouse);
        drawApp->setApplicationCursor(Qt::IBeamCursor);
        break;
    case blur: {
        // 缩放系数公式： 目的系数 = （1-最大系数）/ （最大值 - 最小值）
        double scanleRate = 0.5 / (500 - 5);
        int blur_width = CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getBlurWidth();
        scanleRate = scanleRate * blur_width + 1.0;
        QPixmap pix = /*QPixmap(":/cursorIcons/smudge_mouse.png")*/m_blurMouse.pixmap();
        pix = pix.scaled(static_cast<int>(pix.width() * scanleRate), static_cast<int>(pix.height() * scanleRate));
        drawApp->setApplicationCursor(pix);
        break;
    }
    case cut:
        drawApp->setApplicationCursor(QCursor(Qt::ArrowCursor));
        break;

    default:
        drawApp->setApplicationCursor(QCursor(Qt::ArrowCursor));
        break;

    }
}

//void CDrawScene::clearMutiSelectedState()
//{
//    m_pGroupItem->clear();
//}

void CDrawScene::setDrawForeground(bool b)
{
    bDrawForeground = b;
}

//bool CDrawScene::isDrawedForeground()
//{
//    return bDrawForeground;
//}

void CDrawScene::setItemsActive(bool canSelecte)
{
    //让其他图元不可选中
    QList<QGraphicsItem *> itemList = this->items();
    foreach (QGraphicsItem *item, itemList) {
        if (item->type() > QGraphicsItem::UserType) {
            item->setFlag(QGraphicsItem::ItemIsMovable, canSelecte);
            item->setFlag(QGraphicsItem::ItemIsSelectable, canSelecte);
        }
    }
}

void CDrawScene::blockUpdateBlurItem(bool b)
{
    blockMscUpdate = b;
}

//void CDrawScene::updateBlurItem(QGraphicsItem *changeItem)
//{
//    Q_UNUSED(changeItem)

//    if (blockMscUpdate)
//        return;

//    QList<CGraphicsItem *> lists = getBzItems();
//    foreach (CGraphicsItem *item, lists) {
//        if (item->type() == BlurType) {
//            static_cast<CGraphicsMasicoItem *>(item)->updateMasicPixmap();
//        }
//    }
//}

void CDrawScene::switchTheme(int type)
{
    Q_UNUSED(type);
//    QList<QGraphicsItem *> items = this->items();//this->collidingItems();
//    //QList<QGraphicsItem *> items = this->collidingItems();
//    for (int i = items.size() - 1; i >= 0; i--) {
//        CGraphicsItem *pItem = dynamic_cast<CGraphicsItem *>(items[i]);
//        if (pItem != nullptr) {
//            if (pItem->type() == BlurType) {
//                static_cast<CGraphicsMasicoItem *>(items[i])->updateMasicPixmap();
//            }
//        }
//    }
}

CGraphicsItemGroup *CDrawScene::selectGroup()
{
    return m_pSelGroupItem;
}

CDrawParamSigleton *CDrawScene::getDrawParam()
{
    return m_drawParam;
}

bool CDrawScene::isModified() const
{
    return m_drawParam->isModified();
}

void CDrawScene::setModify(bool isModify)
{
    emit signalIsModify(isModify);
}

void CDrawScene::addCItem(QGraphicsItem *pItem, bool calZ)
{
    if (pItem == nullptr)
        return;

    if (calZ && isBussizeItem(pItem) && !blockZAssign) {
        qreal curMax = getMaxZValue();
        qreal z = curMax + 1;
        pItem->setZValue(z);
    }

    this->addItem(pItem);
}

void CDrawScene::removeCItem(QGraphicsItem *pItem)
{
    if (pItem == nullptr)
        return;

    if (pItem->scene() != this)
        return;

    if (this->isBussizeItem(pItem) || pItem->type() == MgrType)
        static_cast<CGraphicsItem *>(pItem)->setBzGroup(nullptr);

    this->removeItem(pItem);
}

void CDrawScene::blockAssignZValue(bool b)
{
    blockZAssign = b;
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
    }

    return false;
}

//bool CDrawScene::isBzAssicaitedItem(QGraphicsItem *pItem)
//{
//    return (isBussizeItem(pItem) || isBussizeHandleNodeItem(pItem));
//}

bool CDrawScene::isNormalGroupItem(QGraphicsItem *pItem)
{
    if (pItem == nullptr)
        return false;

    if (pItem->type() != MgrType) {
        return false;
    }
    return ((static_cast<CGraphicsItemGroup *>(pItem))->groupType() == CGraphicsItemGroup::ENormalGroup);
}

bool CDrawScene::isDrawItem(QGraphicsItem *pItem)
{
    return (isNormalGroupItem(pItem) || isBussizeItem(pItem));
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

void CDrawScene::clearSelectGroup()
{
    clearSelection();
    m_pSelGroupItem->clear();
}

void CDrawScene::selectItem(QGraphicsItem *pItem, bool onlyBzItem, bool updateAttri, bool updateRect)
{
    if (blockSel)
        return;

    if (pItem == nullptr || pItem->scene() == nullptr)
        return;

    if ((onlyBzItem && isBussizeItem(pItem)) || isNormalGroupItem(pItem)) {
        pItem = static_cast<CGraphicsItem *>(pItem)->thisBzProxyItem(true);
        pItem->setSelected(true);
        m_pSelGroupItem->add(dynamic_cast<CGraphicsItem *>(pItem), updateAttri, updateRect);
    } else {
        pItem->setSelected(true);
    }
}

void CDrawScene::notSelectItem(QGraphicsItem *pItem, bool updateAttri, bool updateRect)
{
    pItem->setSelected(false);

    if (isBussizeItem(pItem) || isNormalGroupItem(pItem)) {
        m_pSelGroupItem->remove(dynamic_cast<CGraphicsItem *>(pItem), updateAttri, updateRect);
    }
}

void CDrawScene::blockSelect(bool b)
{
    blockSel = b;
}

void CDrawScene::selectItemsByRect(const QRectF &rect, bool replace, bool onlyBzItem)
{
    if (blockSel)
        return;

    if (replace)
        clearSelectGroup();

    QList<QGraphicsItem *> itemlists = this->items(rect);

    for (QGraphicsItem *pItem : itemlists) {
        if (onlyBzItem && isBussizeItem(pItem)) {

            CGraphicsItem *pCItem = dynamic_cast<CGraphicsItem *>(pItem)->thisBzProxyItem(true);

            pCItem->setSelected(true);

            // 此处可以不用刷新属性,但是文字图元修改为不同样式后导入画板进行框选,显示的属性不对,后续进行改进
            m_pSelGroupItem->add(pCItem, false, false);
        }
    }
    m_pSelGroupItem->updateAttributes();
    m_pSelGroupItem->updateBoundingRect();

    m_pSelGroupItem->setAddType(CGraphicsItemGroup::ERectSelect);
}

void CDrawScene::updateMrItemBoundingRect()
{
    m_pSelGroupItem->updateBoundingRect();
}

QList<CGraphicsItem *> CDrawScene::getBzItems(const QList<QGraphicsItem *> &items, ESortItemTp tp)
{
    bool isSort = false;
    QList<QGraphicsItem *> lists = items;
    if (lists.isEmpty()) {
        lists = this->items(tp == EDesSort ? Qt::DescendingOrder : Qt::AscendingOrder);
        isSort = true;
    }

    QList<CGraphicsItem *> result;
    for (int i = 0; i < lists.count(); ++i) {
        QGraphicsItem *pItem = lists[i];
        if (isBussizeItem(pItem)) {
            result.append(static_cast<CGraphicsItem *>(pItem));
        }
    }
    if (!isSort) {
        return returnSortZItems(result, tp);
    }
    return result;
}

QList<CGraphicsItem *> CDrawScene::getRootItems(CDrawScene::ESortItemTp tp)
{
    QList<QGraphicsItem *> lists = items(tp == EDesSort ? Qt::DescendingOrder : Qt::AscendingOrder);
    QList<CGraphicsItem *> result;
    for (auto p : lists) {
        if (isBussizeItem(p) || isNormalGroupItem(p)) {
            CGraphicsItem *pC = static_cast<CGraphicsItem *>(p);
            if (pC->bzGroup(true) == nullptr) {
                result.append(pC);
            }
        }
    }
    sortZ(result, tp);
    return result;
}

void CDrawScene::moveBzItemsLayer(const QList<CGraphicsItem *> &items,
                                  EZMoveType tp, int step,
                                  CGraphicsItem *pBaseInGroup,
                                  bool pushToStack)
{
    if (items.isEmpty())
        return;

    if (pushToStack) {
        //图元被加入组合时z值会发生变化所以记录z值信息
        this->recordItemsInfoToCmd(getBzItems(), UndoVar, true);
    }

    switch (tp) {
    case EDownLayer: {
        moveItemsZDown(items, step);
        break;
    }
    case EUpLayer: {
        //sortZOnItemsMove(items, tp, step);
        moveItemsZUp(items, step);
        break;
    }
    case EToGroup: {
        sortZBaseOneBzItem(items, pBaseInGroup);
        break;
    }
    }

    if (pushToStack) {
        //图元被加入组合时z值会发生变化所以记录z值信息
        this->recordItemsInfoToCmd(getBzItems(), RedoVar, false);
        this->finishRecord();
    }
}

//bool CDrawScene::isZMovable(const QList<CGraphicsItem *> &items,
//                            EZMoveType tp,
//                            int step,
//                            CGraphicsItem *pBaseInGroup)
//{
//    if (items.isEmpty())
//        return false;

//    bool result = false;

//    switch (tp) {
//    case EDownLayer:

//    case EUpLayer: {

//        Q_UNUSED(step)

//        CGraphicsItemGroup *pSameGroup = getSameGroup(items, false, false, true);

//        //不支持不处于同一组合下的图层操作
//        result = (pSameGroup != nullptr);

//        if (pSameGroup != nullptr && pSameGroup->groupType() == CGraphicsItemGroup::EVirRootGroup) {
//            delete pSameGroup;
//        }

//        break;
//    }
//    case EToGroup: {
//        result = items.contains(pBaseInGroup);
//        break;
//    }
//    }

//    return result;
//}

bool CDrawScene::isCurrentZMovable(EZMoveType tp, int step, CGraphicsItem *pBaseInGroup)
{
    Q_UNUSED(step)
    Q_UNUSED(pBaseInGroup)

    bool selectcCount = getBzItems().count() >= 2 &&  selectGroup()->count() > 0 &&  selectGroup()->getBzItems(true).count() < getBzItems().count();
    if (!selectcCount) {
        return  false;
    }

    bool result = false;

    // 获取框选所有图元且进行降序排列
    QList<CGraphicsItem *> resultSort = selectGroup()->getBzItems(true);
    auto allBzItemInSelectGroup = returnSortZItems(resultSort, ESortItemTp::EDesSort);

    switch (tp) {
    case EDownLayer: {
        if (allBzItemInSelectGroup.isEmpty())
            return  false;

        CGraphicsItem *minItems = nullptr;
        minItems  = allBzItemInSelectGroup.last();
        if (minItems != nullptr) {
            qreal min = minItems->zValue();
            if (static_cast<int >(min) <= static_cast<int >(getBzItems().last()->zValue()))
                result = false;
            else
                result = true;
        }

        break;
    }

    case EUpLayer: {
        if (allBzItemInSelectGroup.isEmpty())
            return  false;

        CGraphicsItem *maxItems = nullptr;
        maxItems  = allBzItemInSelectGroup.first();
        if (maxItems != nullptr) {
            qreal max = maxItems->zValue();
            if (static_cast<int >(max) >= static_cast<int >(getBzItems().first()->zValue()))
                result = false;
            else
                result = true;
        }

        break;
    }
    case EToGroup:
        break;
    }

    return result;
}

qreal CDrawScene::getMaxZValue()
{
    auto bzItems = getBzItems();
    return bzItems.isEmpty() ? -1 : bzItems.first()->zValue();
}

//降序排列用
bool zValueSortDES(QGraphicsItem *info1, QGraphicsItem *info2)
{
    if (CDrawScene::isDrawItem(info1) && CDrawScene::isDrawItem(info2))
        return static_cast<CGraphicsItem *>(info1)->drawZValue() >= static_cast<CGraphicsItem *>(info2)->drawZValue();
    return info1->zValue() >= info2->zValue();
}
//升序排列用
bool zValueSortASC(QGraphicsItem *info1, QGraphicsItem *info2)
{
    if (CDrawScene::isDrawItem(info1) && CDrawScene::isDrawItem(info2))
        return static_cast<CGraphicsItem *>(info1)->drawZValue() <= static_cast<CGraphicsItem *>(info2)->drawZValue();
    return info1->zValue() <= info2->zValue();
}

CGraphicsItem *CDrawScene::topBzItem(const QPointF &pos, bool penalgor, int IncW)
{
    return dynamic_cast<CGraphicsItem *>(firstItem(pos, QList<QGraphicsItem *>(),
                                                   true, penalgor, true, true, true, IncW));
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
            items = this->items(QRectF(pos - QPoint(incW, incW), 2 * QSize(incW, incW)), Qt::IntersectsItemShape);
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
                CGraphicsItemGroup *pSelctMrItem = dynamic_cast<CGraphicsItemGroup *>(pItem);
                QList<CGraphicsItem *> selects = pSelctMrItem->items();
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
                bool isPenetrable = pBzItem->isPosPenetrable(pBzItem->mapFromScene(pos));
                if (incW != 0) {
                    QRectF rect(pBzItem->mapFromScene(pos) - QPoint(incW, incW), 2 * QSize(incW, incW));
                    isPenetrable = pBzItem->isRectPenetrable(rect);
                }
                if (!isPenetrable) {
                    //在该位置不透明,判定完成
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

// Cppcheck检测函数没有使用到
//QList<CGraphicsItem *> CDrawScene::findBzItems(const QPointF &pos,
//                                               bool seeNodeAsBzItem,
//                                               bool filterGroup,
//                                               int incW)
//{
//    QList<QGraphicsItem *> items;
//    if (incW == 0) {
//        items = this->items(pos);
//    } else {
//        items = this->items(QRectF(pos - QPoint(incW, incW), 2 * QSize(incW, incW)), Qt::IntersectsItemShape);
//    }
//    QList<CGraphicsItem *> resultItems;
//    for (auto p : items) {
//        if (isDrawItem(p)) {
//            if (filterGroup) {
//                if (!isNormalGroupItem(p)) {
//                    resultItems.append(static_cast<CGraphicsItem *>(p));
//                }
//            } else {
//                resultItems.append(static_cast<CGraphicsItem *>(p));
//            }
//        } else {
//            if (seeNodeAsBzItem && isBussizeHandleNodeItem(p)) {
//                auto pItem = getAssociatedBzItem(p);
//                if (filterGroup) {
//                    if (!isNormalGroupItem(pItem)) {
//                        resultItems.append(pItem);
//                    }
//                } else {
//                    resultItems.append(pItem);
//                }
//            }
//        }
//    }
//    return resultItems;
//}

void CDrawScene::blockMouseMoveEvent(bool b)
{
    blockMouseMoveEventFlag = b;
}

bool CDrawScene::isBlockMouseMoveEvent()
{
    return blockMouseMoveEventFlag;
}

void CDrawScene::recordSecenInfoToCmd(int exptype, EVarUndoOrRedo varFor, const QList<CGraphicsItem *> &items)
{
    QList<QVariant> vars;
    vars << reinterpret_cast<long long>(this);

    QVariant var;
    CSceneUndoRedoCommand::EChangedType tp = CSceneUndoRedoCommand::EChangedType(exptype);

    //当前仅支持EGroupChanged的场景快照还原
    switch (tp) {
    case CSceneUndoRedoCommand::EGroupChanged: {
        var.setValue<CGroupBzItemsTree>(getGroupTree());
        vars << var;

        //真正的变动图元,可以用于撤销还原后的选中操作
        for (QGraphicsItem *pItem : items) {
            vars << reinterpret_cast<long long>(pItem);
        }

        break;
    }
    default:
        return;
    }

    if (varFor == UndoVar) {
        CUndoRedoCommand::recordUndoCommand(CUndoRedoCommand::ESceneChangedCmd, exptype,
                                            vars, true, false);
    } else if (varFor == RedoVar) {
        CUndoRedoCommand::recordRedoCommand(CUndoRedoCommand::ESceneChangedCmd, exptype,
                                            vars);
    }
}

void CDrawScene::recordItemsInfoToCmd(const QList<CGraphicsItem *> &items, EVarUndoOrRedo varFor, bool clearInfo)
{
    //qWarning() << "recordItemsInfoToCmd ============ " << int(varFor);
    for (int i = 0; i < items.size(); ++i) {
        CGraphicsItem *pItem = items[i];
        QList<QVariant> vars;
        vars << reinterpret_cast<long long>(pItem);
        QVariant varInfo;
        varInfo.setValue(pItem->getGraphicsUnit(EUndoRedo));
        vars << varInfo;

        if (varFor == UndoVar) {
            CUndoRedoCommand::recordUndoCommand(CUndoRedoCommand::EItemChangedCmd,
                                                CItemUndoRedoCommand::EAllChanged, vars, clearInfo && (i == 0), false);
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

bool CDrawScene::isGroupable(const QList<CGraphicsItem *> &pBzItems)
{
    QList<CGraphicsItem *> bzItems = pBzItems;
    if (pBzItems.isEmpty()) {
        bzItems = selectGroup()->items();
    }

    //组合个数必须是大于1的
    if (bzItems.count() <= 1)
        return false;

    return true;
}

bool CDrawScene::isUnGroupable(const QList<CGraphicsItem *> &pBzItems)
{
    QList<CGraphicsItem *> bzItems = pBzItems;
    if (pBzItems.isEmpty()) {
        bzItems = selectGroup()->items();
    }

    for (auto pItem : bzItems) {
        pItem = pItem->thisBzProxyItem(true);
        if (isNormalGroupItem(pItem))
            return  true;
    }

    return false;
}

CGraphicsItemGroup *CDrawScene::getSameGroup(const QList<CGraphicsItem *> &pBzItems, bool top,
                                             bool onlyNormal,
                                             bool sameNullCreatVirGroup)
{
    if (pBzItems.isEmpty())
        return nullptr;

    CGraphicsItemGroup *pGroup = top ? pBzItems.first()->bzTopGroup(onlyNormal) : pBzItems.first()->bzGroup(onlyNormal);
    bool isSameNullptr = (pGroup == nullptr);
    for (int i = 1; i < pBzItems.size(); ++i) {
        CGraphicsItem *p = pBzItems[i];
        CGraphicsItemGroup *pTopGroupTemp = top ? p->bzTopGroup(onlyNormal) : p->bzGroup(onlyNormal);
        if (pGroup != pTopGroupTemp) {
            pGroup = nullptr;
            isSameNullptr = false;
            break;
        }
    }

    if (isSameNullptr && !top && sameNullCreatVirGroup) {
        //创建一个虚拟管理组合,用完记得删除
        CGraphicsItemGroup *pNewGroup = new CGraphicsItemGroup;
        for (auto p : pBzItems) {
            pNewGroup->add(p, false, false);
        }
        pNewGroup->setGroupType(CGraphicsItemGroup::EVirRootGroup);
        return pNewGroup;
    }

    return /*nullptr*/pGroup;
}

CGraphicsItemGroup *CDrawScene::creatGroup(const QList<CGraphicsItem *> &pBzItems,
                                           int groupType,
                                           bool pushUndo,
                                           CGraphicsItem *pBzItemZBase, bool getMinZItemIfNull)
{
    QList<CGraphicsItem *> bzItems = pBzItems;
    if (pBzItems.isEmpty()) {
        bzItems = selectGroup()->items();
    }
    if (bzItems.count() == 0)
        return nullptr;

    //普通组合个数必须是大于1的,其他特殊的组合没有此限制
    if (groupType == CGraphicsItemGroup::ENormalGroup && bzItems.count() <= 1)
        return nullptr;

    //pBzItems中不能有图元在某个组合下
    for (auto p : pBzItems) {
        if (p->bzGroup() != nullptr)
            return nullptr;
    }

    CCmdBlock block(pushUndo ? this : nullptr, CSceneUndoRedoCommand::EGroupChanged,
                    CGraphicsItem::returnList(bzItems));
    if (pushUndo) {
        //图元被加入组合时z值会发生变化所以记录z值信息
        this->recordItemsInfoToCmd(getBzItems(), UndoVar, false);
    }
    //CGraphicsItemGroup *pNewGroup = m_pCachGroups.isEmpty() ? new CGraphicsItemGroup : m_pCachGroups.takeFirst();
    CGraphicsItemGroup *pNewGroup = new CGraphicsItemGroup;

    pNewGroup->setGroupType(CGraphicsItemGroup::EGroupType(groupType));

    for (auto it : bzItems) {
        it->setBzGroup(pNewGroup);
    }

    pNewGroup->setRecursiveScene(this);

    pNewGroup->updateBoundingRect();

    if (!m_pGroups.contains(pNewGroup))
        m_pGroups.append(pNewGroup);

    selectItem(pNewGroup);

    qDebug() << "in used groups count = " << m_pGroups.count() << "cached groups = " << m_pCachGroups.count();

    //排序z
    if (pBzItemZBase == nullptr && getMinZItemIfNull) {
        pBzItemZBase = CGraphicsItem::zItem(bzItems);
    }
    if (pBzItemZBase != nullptr) {
        sortZBaseOneBzItem(pNewGroup->getBzItems(true), pBzItemZBase);
    }

    if (pushUndo) {
        //图元被加入组合时z值会发生变化所以记录z值信息
        this->recordItemsInfoToCmd(getBzItems(), RedoVar, false);
    }

    return pNewGroup;
}

CGraphicsItemGroup *CDrawScene::copyCreatGroup(CGraphicsItemGroup *pGroup)
{
    if (pGroup == nullptr || pGroup->drawScene() == nullptr)
        return nullptr;

    CDrawScene *pScene = pGroup->drawScene();
    CGroupBzItemsTreeInfo itemsTreeInfo = pScene->getGroupTreeInfo(pGroup);
    CGraphicsItemGroup *pNewGroup = pScene->loadGroupTreeInfo(itemsTreeInfo, true);
    releaseBzItemsTreeInfo(itemsTreeInfo);
    return pNewGroup;
}

void CDrawScene::cancelGroup(CGraphicsItemGroup *pGroup, bool pushUndo)
{
    QList<CGraphicsItem *> itemlists;
    if (pGroup == nullptr) {
        QList<CGraphicsItem *> bzItems = selectGroup()->items();
        QList<CGraphicsItemGroup *> pGroups;
        for (auto pItem : bzItems) {
            pItem = pItem->thisBzProxyItem(true);
            if (isNormalGroupItem(pItem)) {
                pGroup = static_cast<CGraphicsItemGroup *>(pItem);
                if (pGroup != nullptr && pGroup->isCancelable()) {
                    itemlists.append(pGroup->items());
                    pGroups.append(pGroup);
                }

            }
        }
        CCmdBlock block(pushUndo ? this : nullptr, CSceneUndoRedoCommand::EGroupChanged,
                        CGraphicsItem::returnList(itemlists));

        for (auto pGroup : pGroups) {
            destoryGroup(pGroup, false, false);
        }
        qDebug() << "in used groups count = " << m_pGroups.count() << "cached groups = " << m_pCachGroups.count();
    } else {
        if (pGroup->isCancelable()) {
            itemlists = pGroup->items();
            destoryGroup(pGroup, pGroup->groupType() == CGraphicsItemGroup::EVirRootGroup, pushUndo);
        }
    }

    if (!itemlists.isEmpty())
        clearSelectGroup();
    // 取消组合需要还原框选状态
    for (CGraphicsItem *pCItem : itemlists) {
        selectItem(pCItem, true, false, false);
    }
    // 更新边界矩形框
    m_pSelGroupItem->updateAttributes();
    m_pSelGroupItem->updateBoundingRect();
    m_pSelGroupItem->setAddType(CGraphicsItemGroup::ERectSelect);
}

void CDrawScene::destoryGroup(CGraphicsItemGroup *pGroup, bool deleteIt, bool pushUndo)
{
    if (pGroup == nullptr)
        return;

    //保证组合是这个场景的
    assert(pGroup->scene() == this);

    CCmdBlock block(pushUndo ? this : nullptr, CSceneUndoRedoCommand::EGroupChanged, CGraphicsItem::returnList(pGroup->items()));

    if (pGroup->isSelected())
        notSelectItem(pGroup);

    this->removeCItem(pGroup);

    m_pGroups.removeOne(pGroup);

    pGroup->clear();

    pGroup->setGroupType(CGraphicsItemGroup::ENormalGroup);

    Q_UNUSED(deleteIt)
    /*    if (deleteIt) {
            m_pCachGroups.removeOne(pGroup);
            delete pGroup;
        } else*/ {
        pGroup->resetTransform();
        m_pCachGroups.append(pGroup);
    }
}

void CDrawScene::destoryAllGroup(bool deleteIt, bool pushUndo)
{
    CCmdBlock block(pushUndo ? this : nullptr, CSceneUndoRedoCommand::EGroupChanged);
    while (!m_pGroups.isEmpty()) {
        destoryGroup(m_pGroups.takeFirst(), deleteIt);
    }
}

//CGraphicsItemGroup *CDrawScene::getGroup(CGraphicsItem *pBzItem)
//{
//    if (pBzItem == nullptr) {
//        return nullptr;
//    }
//    return pBzItem->bzGroup();
//}

QList<CGraphicsItemGroup *> CDrawScene::bzGroups()
{
    return m_pGroups;
}
