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
#include "cselecttool.h"
#include "cdrawscene.h"
#include "cgraphicsitem.h"
#include "cgraphicslineitem.h"
#include "cdrawparamsigleton.h"
#include "cgraphicsrotateangleitem.h"
#include "cgraphicstextitem.h"
#include "cgraphicsproxywidget.h"
#include "cgraphicscutitem.h"
#include "cgraphicsellipseitem.h"
#include "cgraphicstriangleitem.h"
#include "cgraphicspolygonitem.h"
#include "cgraphicspolygonalstaritem.h"
#include "widgets/ctextedit.h"
#include "cgraphicsmasicoitem.h"
#include "drawshape/cpictureitem.h"
#include "cgraphicsitemselectedmgr.h"
#include "cgraphicsitemhighlight.h"
#include "frame/cviewmanagement.h"
#include "frame/cgraphicsview.h"
#include "frame/cundocommands.h"
#include "application.h"
#include "frame/cundoredocommand.h"

#include "service/cmanagerattributeservice.h"

#include <DApplication>

#include <QGraphicsSceneMouseEvent>
#include <QDebug>
#include <QCursor>
#include <QtMath>
#include <QGraphicsTextItem>
#include <QTextCursor>
#include <QPixmap>
#include <QGraphicsView>
#include <QTextEdit>
#include <QSvgGenerator>
#include <QtMath>

CSelectTool::CSelectTool()
    : IDrawTool(selection)
    , m_textEditCursor(QPixmap(":/theme/light/images/mouse_style/text_mouse.svg"))
    , m_isItemMoving(false)
{
}

CSelectTool::~CSelectTool()
{

}

bool CSelectTool::isDragging()
{
    return m_isItemMoving;
}

void CSelectTool::mousePressEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene)
{
    return IDrawTool::mousePressEvent(event, scene);
}

void CSelectTool::mouseMoveEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene)
{
    return IDrawTool::mouseMoveEvent(event, scene);
}

void CSelectTool::mouseReleaseEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene)
{
    return IDrawTool::mouseReleaseEvent(event, scene);
}

void CSelectTool::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene)
{
    Q_UNUSED(event)
    Q_UNUSED(scene)
}

void CSelectTool::toolStart(IDrawTool::CDrawToolEvent *event, ITERecordInfo *pInfo)
{
    _hightLight = QPainterPath();

    QGraphicsItem *pStartPostTopBzItem = pInfo->startPosTopBzItem;
    QGraphicsItem *pSelctFirBzItem = event->scene()->firstBzItem(event->scene()->selectedItems(), true);

    QGraphicsItem *pFirstItem = pInfo->startPosItems.isEmpty() ? nullptr : pInfo->startPosItems.first();
    bool isMrNodeItem = event->scene()->isBussizeHandleNodeItem(pFirstItem) && (event->scene()->getAssociatedBzItem(pFirstItem)->type() == MgrType);

    bool doSelect = false;
    bool clearBeforeSelect = true;

    //判断在函数返回前是否要执行选中操作
    if (event->eventType() == CDrawToolEvent::EMouseEvent) {
        if (event->mouseButtons() == Qt::LeftButton) {
            doSelect = true;
        }

        bool notSameItem = (event->scene()->selectedItems().count() == 1 && pStartPostTopBzItem != pSelctFirBzItem);

        //只有在没点住shift键和但前选中的个数为1并且不是但前鼠标下的item时才清除当前选中
        clearBeforeSelect = (event->keyboardModifiers() != Qt::ShiftModifier) && notSameItem && (!isMrNodeItem);

    } else if (event->eventType() == CDrawToolEvent::ETouchEvent) {
        doSelect = true;
    }

    if (clearBeforeSelect) {
        event->scene()->clearMrSelection();
    }

    if (doSelect) {
        if (pStartPostTopBzItem != nullptr) {
            event->scene()->selectItem(pStartPostTopBzItem);
            event->setAccepted(true);
        } else {
            if (!isMrNodeItem) {
                //点击处是空白的那么清理所有选中

                event->scene()->clearMrSelection();
            }
            event->setAccepted(true);
        }
    }
}

void CSelectTool::toolUpdate(IDrawTool::CDrawToolEvent *event, ITERecordInfo *pInfo)
{
    event->setAccepted(false);

    //根据要做的类型去执行相应的操作
    switch (pInfo->_opeTpUpdate) {
    case ERectSelect: {
        QPointF pos0 = pInfo->_startPos;
        QPointF pos1 = event->pos();

        QPointF topLeft(qMin(pos0.x(), pos1.x()), qMin(pos0.y(), pos1.y()));
        QPointF bomRight(qMax(pos0.x(), pos1.x()), qMax(pos0.y(), pos1.y()));

        event->scene()->selectItemsByRect(QRectF(topLeft, bomRight));

        event->scene()->update();
        break;
    }
    case EDragMove: {
        //执行移动操作
        for (QGraphicsItem *pItem : pInfo->etcItems) {
            if (event->scene()->isBussizeItem(pItem) || pItem->type() == MgrType) {
                CGraphicsItem *pBzItem = dynamic_cast<CGraphicsItem *>(pItem);
                pBzItem->move(pInfo->_prePos, event->pos());
            }
        }
        event->scene()->update();
        event->setAccepted(true);
        break;
    }
    case EResizeMove: {
        CSizeHandleRect::EDirection dir = CSizeHandleRect::EDirection(pInfo->_etcopeTpUpdate);

        if (dir != CSizeHandleRect::Rotation) {
            for (QGraphicsItem *pItem : pInfo->etcItems) {
                if (event->scene()->isBussizeItem(pItem) || pItem->type() == MgrType) {
                    CGraphicsItem *pBzItem = dynamic_cast<CGraphicsItem *>(pItem);
                    pBzItem->newResizeTo(dir, event->pos(), event->pos() - pInfo->_prePos,
                                         event->keyboardModifiers() == Qt::ShiftModifier, false);
                }
            }
        } else {
            if (!pInfo->etcItems.isEmpty()) {
                QGraphicsItem *pItem = !pInfo->etcItems.isEmpty() ? pInfo->etcItems.first() : nullptr;
                CGraphicsItem *pMrItem = dynamic_cast<CGraphicsItem *>(pItem);
                QPointF center = pMrItem->rect().center();
                QPointF mousePoint = event->pos();
                QPointF centerToScence = pMrItem->mapToScene(center);
                qreal len_y = mousePoint.y() - centerToScence.y();
                qreal len_x = mousePoint.x() - centerToScence.x();
                qreal angle = atan2(-len_x, len_y) * 180 / M_PI + 180;
                pMrItem->rotatAngle(angle);
            }
        }
        break;
    }
    case ECopyMove: {
        //复制移动
        if (!pInfo->etcItems.isEmpty()) {
            QPointF move = event->pos() - pInfo->_prePos;
            event->scene()->moveItems(pInfo->etcItems, move);
        }
        break;
    }
    default:
        break;
    }

    //IDrawTool::toolUpdate(event, pInfo);
}

void CSelectTool::toolFinish(IDrawTool::CDrawToolEvent *event, ITERecordInfo *pInfo)
{
    switch (pInfo->_opeTpUpdate) {
    case ERectSelect: {
        break;
    }
    case EDragMove: {
        QList<CGraphicsItem *> lists = event->scene()->getItemsMgr()->getItems();
        lists.push_front(event->scene()->getItemsMgr());
        //记录还原点，之后入栈
        for (int i = 0; i < lists.size(); ++i) {
            QGraphicsItem *pItem = lists[i];

            QList<QVariant> vars;
            vars << reinterpret_cast<long long>(pItem);
            vars << pItem->scenePos();

            CUndoRedoCommand::recordRedoCommand(CUndoRedoCommand::EItemChangedCmd,
                                                CItemUndoRedoCommand::EPosChanged, vars);
        }
        m_isItemMoving = false;
        break;
    }
    case EResizeMove: {
        break;
    }
    case ECopyMove: {
        //记录撤销点
        QList<QVariant> vars;
        vars << reinterpret_cast<long long>(event->scene());
        for (int i = 0; i < pInfo->etcItems.size(); ++i) {
            QGraphicsItem *pItem = pInfo->etcItems[i];
            vars << reinterpret_cast<long long>(pItem);
        }
        CUndoRedoCommand::recordRedoCommand(CUndoRedoCommand::ESceneChangedCmd,
                                            CSceneUndoRedoCommand::EItemAdded, vars);
        break;
    }
    default:
        break;
    }

    //入栈
    CUndoRedoCommand::finishRecord();

    IDrawTool::toolFinish(event, pInfo);
}

int CSelectTool::decideUpdate(IDrawTool::CDrawToolEvent *event, IDrawTool::ITERecordInfo *pInfo)
{
    int tpye = ENothingDo;
    if (m_bMousePress) {
        QGraphicsItem *pStartPosTopQtItem = event->scene()->firstItem(pInfo->_startPos,
                                                                      pInfo->startPosItems, true, true);
        if (pStartPosTopQtItem == nullptr) {
            tpye = ERectSelect;
        } else {
            if (event->scene()->isBussizeItem(pStartPosTopQtItem)) {
                // 业务图元可执行移动可执行复制
                if (event->keyboardModifiers() == Qt::AltModifier) {
                    //复制移动
                    tpye = ECopyMove;
                    pInfo->etcItems = copyItemsToScene(event->scene()->getBzItems(event->scene()->selectedItems()),
                                                       event->scene());

                    //记录还原点，之后入栈
                    QList<QVariant> vars;
                    vars << reinterpret_cast<long long>(event->scene());
                    for (int i = 0; i < pInfo->etcItems.size(); ++i) {
                        QGraphicsItem *pItem = pInfo->etcItems[i];
                        vars << reinterpret_cast<long long>(pItem);
                    }
                    CUndoRedoCommand::recordUndoCommand(CUndoRedoCommand::ESceneChangedCmd,
                                                        CSceneUndoRedoCommand::EItemAdded, vars, true, true);
                } else {
                    tpye = EDragMove;

                    pInfo->etcItems.append(event->scene()->getItemsMgr());

                    QList<CGraphicsItem *> lists = event->scene()->getItemsMgr()->getItems();
                    lists.push_front(event->scene()->getItemsMgr());

                    //记录还原点，之后入栈
                    for (int i = 0; i < lists.size(); ++i) {
                        CGraphicsItem *pItem = lists[i];

                        QList<QVariant> vars;
                        vars << reinterpret_cast<long long>(pItem);
                        vars << pItem->scenePos();

                        CUndoRedoCommand::recordUndoCommand(CUndoRedoCommand::EItemChangedCmd,
                                                            CItemUndoRedoCommand::EPosChanged, vars, true, i == 0);
                    }
                    m_isItemMoving = true;
                }
            } else if (event->scene()->isBussizeHandleNodeItem(pStartPosTopQtItem)) {
                tpye = EResizeMove;
                CSizeHandleRect *pHandle = dynamic_cast<CSizeHandleRect *>(pStartPosTopQtItem);
                pInfo->_etcopeTpUpdate = pHandle->dir();
                pInfo->etcItems.clear();
                if (pHandle->dir() == CSizeHandleRect::Rotation) {
                    pInfo->etcItems.append(event->scene()->getItemsMgr());
                } else {
                    pInfo->etcItems.append(event->scene()->getItemsMgr());
                }
            }
        }
    }
    return tpye;
}

void CSelectTool::mouseHoverEvent(IDrawTool::CDrawToolEvent *event)
{
    //处理高亮，鼠标样式变化等问题

    _hightLight = QPainterPath();
    QList<QGraphicsItem *> items = event->scene()->items(event->pos());

    QGraphicsItem *pItem = event->scene()->firstItem(event->pos(), items, true, true, false, false);
    CGraphicsItem *pBzItem = dynamic_cast<CGraphicsItem *>(event->scene()->firstItem(event->pos(), items,
                                                                                     true, true, true, true));

    if (pBzItem != nullptr) {
        _hightLight = pBzItem->mapToScene(pBzItem->getHighLightPath());
    }

    if (event->scene()->isBussizeHandleNodeItem(pItem)) {
        CSizeHandleRect *pHandle = dynamic_cast<CSizeHandleRect *>(pItem);
        dApp->setApplicationCursor(QCursor(getCursor(pHandle->dir(), false, 1)));
    } else {
        dApp->setApplicationCursor(Qt::ArrowCursor);
    }

    event->scene()->update();
}

void CSelectTool::drawMore(QPainter *painter,
                           const QRectF &rect,
                           CDrawScene *scene)
{
    //注意painter是在scene的坐标系

    Q_UNUSED(rect)
    painter->save();
    for (auto it = _allITERecordInfo.begin();
         it != _allITERecordInfo.end(); ++it) {
        ITERecordInfo &info = it.value();
        if (info._opeTpUpdate == ERectSelect) {
            QPointF pos0 = info._startPos;
            QPointF pos1 = info._curEvent.pos();

            QPointF topLeft(qMin(pos0.x(), pos1.x()), qMin(pos0.y(), pos1.y()));
            QPointF bomRight(qMax(pos0.x(), pos1.x()), qMax(pos0.y(), pos1.y()));

            DPalette pa = scene->palette();
            QPen pen;
            pen.setWidthF(0.5);
            QBrush selectBrush = pa.brush(QPalette::Active, DPalette::Highlight);
            QColor selectColor = selectBrush.color();
            selectColor.setAlpha(20);
            selectBrush.setColor(selectColor);
            selectColor.setAlpha(100);
            pen.setColor(selectColor);

            painter->setPen(pen);
            painter->setBrush(selectBrush);
            painter->drawRect(QRectF(topLeft, bomRight));
        }
    }

    if (!_hightLight.isEmpty()) {
        painter->setBrush(Qt::NoBrush);
        QPen p(QColor(255, 0, 0));
        p.setWidthF(2.0);
        painter->setPen(p);
        painter->drawPath(_hightLight);
    }
    painter->restore();
}

QList<QGraphicsItem *> CSelectTool::copyItemsToScene(const QList<QGraphicsItem *> &items,
                                                     CDrawScene *scene)
{
    QList<QGraphicsItem *> zItems = scene->returnSortZItems(items);

    qreal minZ = zItems.last()->zValue();
    qreal maxZ = zItems.first()->zValue();

    //新复制的业务图元基本z值应该是 被复制items里面的最大z值
    qreal newMin = scene->getMaxZValue();

    scene->blockUpdateBlurItem(true);
    QList<QGraphicsItem *> createdItems;
    for (int i = 0; i < zItems.count(); ++i) {
        CGraphicsItem *pBzItem = dynamic_cast<CGraphicsItem *>(zItems[i]);
        if (pBzItem != nullptr) {
            CGraphicsItem *pOutItem = pBzItem->creatSameItem();
            if (pOutItem != nullptr) {
                pOutItem->setZValue(pOutItem->zValue() - minZ + newMin);
                scene->addItem(pOutItem);
                createdItems.append(pOutItem);
            }
        }
    }
    scene->setMaxZValue(newMin + maxZ - minZ);
    scene->blockUpdateBlurItem(false);
    scene->updateBlurItem();
    return createdItems;
}

void CSelectTool::updateCursorShape()
{
    CGraphicsView *view = CManageViewSigleton::GetInstance()->getCurView();
    if (view != nullptr) {
        if (view != nullptr && view->scene() != nullptr) {

            if (view->isKeySpacePressed()) {
                return ;
            }

            QPoint viewPortPos =  view->viewport()->mapFromGlobal(QCursor::pos());
            QPointF scenePos = view->mapToScene(viewPortPos);
            QList<QGraphicsItem *> posItems = view->scene()->items(scenePos);
            if (posItems.isEmpty()) {
                dApp->setApplicationCursor(Qt::ArrowCursor);
            } else {
                QGraphicsItem *pFirstItem = posItems.first();

                //CSizeHandleRect的父类QGraphicsSvgItem的类型就是13
                if (pFirstItem->type() == QGraphicsSvgItem::Type) {
                    CSizeHandleRect *pHandleItem = dynamic_cast<CSizeHandleRect *>(pFirstItem);
                    if (pHandleItem != nullptr) {
                        dApp->setApplicationCursor(getCursor(pHandleItem->dir(), false));
                    } else {
                        dApp->setApplicationCursor(Qt::ArrowCursor);
                    }
                } else if (pFirstItem->type() == QGraphicsProxyWidget::Type) {
                    //QGraphicsProxyWidget的类型就是12
                    QGraphicsProxyWidget *pProxyWidget = dynamic_cast<QGraphicsProxyWidget *>(pFirstItem);
                    if (pProxyWidget != nullptr) {
                        CGraphicsTextItem *pTextItem = dynamic_cast<CGraphicsTextItem *>(pProxyWidget->parentItem());
                        if (pTextItem != nullptr && pTextItem->isEditable()) {
                            dApp->setApplicationCursor(m_textEditCursor);
                        } else {
                            dApp->setApplicationCursor(Qt::ArrowCursor);
                        }
                    } else {
                        dApp->setApplicationCursor(Qt::ArrowCursor);
                    }

                } else {
                    dApp->setApplicationCursor(Qt::ArrowCursor);
                }
            }
        }
    }
}
