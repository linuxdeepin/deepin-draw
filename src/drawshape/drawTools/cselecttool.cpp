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
#include "bzItems/cpictureitem.h"
#include "cgraphicsitemselectedmgr.h"
//#include "cgraphicsitemhighlight.h"
#include "frame/cviewmanagement.h"
#include "frame/cgraphicsview.h"
//#include "frame/cundocommands.h"
#include "application.h"
#include "frame/cundoredocommand.h"
#include "cgraphicsitemevent.h"

#include <DApplication>
#include <QScrollBar>
#include <QGraphicsSceneMouseEvent>
#include <QDebug>
#include <QCursor>
#include <QtMath>
#include <QGraphicsTextItem>
#include <QPainterPath>
#include <QTextCursor>
#include <QPixmap>
#include <QGraphicsView>
#include <QTextEdit>
#include <QSvgGenerator>
#include <QtMath>

CSelectTool::CSelectTool()
    : IDrawTool(selection)
    , m_isItemMoving(false)
{
}

CSelectTool::~CSelectTool()
{

}

void CSelectTool::toolStart(IDrawTool::CDrawToolEvent *event, ITERecordInfo *pInfo)
{
    _hightLight = QPainterPath();

    CGraphicsItem *pStartPosTopBzItem = pInfo->startPosTopBzItem;

    QGraphicsItem *pFirstItem = pInfo->startPosItems.isEmpty() ? nullptr : pInfo->startPosItems.first();
    bool isMrNodeItem = event->scene()->isBussizeHandleNodeItem(pFirstItem) && (event->scene()->getAssociatedBzItem(pFirstItem)->type() == MgrType);

    bool clearBeforeSelect = true;

    if (event->keyboardModifiers() == Qt::ShiftModifier) {
        // 点住shift那么不用清除
        clearBeforeSelect = false;

        //首先应该要满足当前鼠标点击下的图元不是节点
        if (!isMrNodeItem) {
            //如果点击的图元当前是选中的,那么在按住shift的情况下应该是清除它的选中
            if (pStartPosTopBzItem != nullptr) {
                CGraphicsItem *pProxItem = pStartPosTopBzItem->thisBzProxyItem(true);
                if (pProxItem->isSelected()) {
                    if (event->scene()->selectGroup()->count() > 1) {
                        event->scene()->notSelectItem(pProxItem);
                        return;
                    }
                }
            }
        }
    } else {
        if (isMrNodeItem) {
            clearBeforeSelect = false;
        } else if (pStartPosTopBzItem != nullptr) {
            CGraphicsItem *pProxItem = pStartPosTopBzItem->thisBzProxyItem(true);
            if (pProxItem->isSelected()) {
                //其代理图元(自身或者其顶层组合图元)被选中那么不用清除
                clearBeforeSelect = false;
            }
        }
    }
    //清理当前选中
    if (clearBeforeSelect) {
        event->scene()->clearSelectGroup();
    }

    if (pStartPosTopBzItem != nullptr) {
        if (!isMrNodeItem) {
            event->scene()->selectItem(pStartPosTopBzItem->thisBzProxyItem());
        }
    } else {
        if (!isMrNodeItem) {
            //点击处是空白的那么清理所有选中
            event->scene()->clearSelectGroup();
        }
    }
}

void CSelectTool::toolUpdate(IDrawTool::CDrawToolEvent *event, ITERecordInfo *pInfo)
{
    event->setAccepted(false);

    //根据要做的类型去执行相应的操作
    switch (pInfo->_opeTpUpdate) {
    case ERectSelect: {
        event->scene()->update();
        break;
    }
    case EDragSceneMove: {
        //拖拽画布移动
        QPointF pos0 = pInfo->_startPos;
        QPointF pos1 = event->pos();
        QPointF mov = pos1 - pos0;

        QScrollBar *horBar = event->view()->horizontalScrollBar();
        int horValue = horBar->value() - qRound(mov.x());
        horBar->setValue(qMin(qMax(horBar->minimum(), horValue), horBar->maximum()));

        QScrollBar *verBar = event->view()->verticalScrollBar();
        int verValue = verBar->value() - qRound(mov.y());
        verBar->setValue(qMin(qMax(verBar->minimum(), verValue), verBar->maximum()));

        break;
    }
    case EDragMove:
    case ECopyMove: {
        //执行移动操作
        processItemsMove(event, pInfo, EChangedUpdate);
        break;
    }
    case EResizeMove: {
        //交给图元去完成
        processItemsScal(event, pInfo, EChangedUpdate);
        break;
    }
    case ERotateMove: {
        if (!pInfo->etcItems.isEmpty()) {
            QGraphicsItem *pItem = !pInfo->etcItems.isEmpty() ? pInfo->etcItems.first() : nullptr;
            CGraphicsItem *pMrItem = dynamic_cast<CGraphicsItem *>(pItem);
            processItemsRot(event, pInfo, EChangedUpdate);
            drawApp->setApplicationCursor(pMrItem->handleNode()->getCursor());
        }
        break;
    }
    default:
        break;
    }
}

void CSelectTool::toolFinish(IDrawTool::CDrawToolEvent *event, ITERecordInfo *pInfo)
{
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
        event->scene()->recordItemsInfoToCmd(event->scene()->selectGroup()->items(true), RedoVar);
        m_isItemMoving = false;
        break;
    }
    case EResizeMove: {
        //记录Redo点
        event->scene()->recordItemsInfoToCmd(event->scene()->selectGroup()->items(true), RedoVar);
        break;
    }
    case ECopyMove: {
        event->scene()->recordSecenInfoToCmd(CSceneUndoRedoCommand::EGroupChanged, RedoVar, event->scene()->selectGroup()->items());
        m_isItemMoving = false;
        break;
    }
    case ERotateMove: {
        event->scene()->recordItemsInfoToCmd(event->scene()->selectGroup()->items(true), RedoVar);
        break;
    }
    default:
        return;
    }

    QList<CGraphicsItem *> items = event->scene()->selectGroup()->getBzItems(true);
    if (!items.isEmpty()) {
        QGraphicsItem *pItem = items.first();
        if (pItem != nullptr && pItem->type() == PictureType) {
            event->scene()->selectGroup()->updateAttributes();
        }
    }

    //入栈
    CUndoRedoCommand::finishRecord();

    IDrawTool::toolFinish(event, pInfo);
}

void CSelectTool::toolDoubleClikedEvent(IDrawTool::CDrawToolEvent *event, IDrawTool::ITERecordInfo *pInfo)
{
    IDrawTool::toolDoubleClikedEvent(event, pInfo);

    if (pInfo->startPosTopBzItem != nullptr) {
        if (pInfo->startPosTopBzItem->type() == TextType) {
            CGraphicsTextItem *pTextItem = dynamic_cast<CGraphicsTextItem *>(pInfo->startPosTopBzItem);

            //如果是非编辑状态那么应该进入编辑状态
            if (pTextItem->textState() == CGraphicsTextItem::EReadOnly) {
                pTextItem->setTextState(CGraphicsTextItem::EInEdit, true);
            }
        }
    }
    event->setAccepted(true);
}

int CSelectTool::decideUpdate(IDrawTool::CDrawToolEvent *event, IDrawTool::ITERecordInfo *pInfo)
{
    int tpye = ENothingDo;
    if (isActived()) {
        QGraphicsItem *pStartPosTopQtItem = event->scene()->firstItem(pInfo->_startPos,
                                                                      pInfo->startPosItems,
                                                                      true, true);
        if (pStartPosTopQtItem == nullptr) {
            if (event->eventType() == CDrawToolEvent::ETouchEvent) {
                if (pInfo->elapsedFromStartToUpdate() > 200) {
                    tpye = ERectSelect;
                } else {
                    tpye = EDragSceneMove;
                }
            } else {
                tpye = ERectSelect;
            }
        } else {
            if (event->scene()->isBussizeItem(pStartPosTopQtItem)) {
                // 业务图元可执行移动可执行复制
                if (event->keyboardModifiers() == Qt::AltModifier) {
                    //复制移动
                    tpye = ECopyMove;
                    if (event->scene()->selectGroup()->count() > 0) {

                        //0.复制前得到当前场景的组合快照(用于Undo撤销)
                        event->scene()->recordSecenInfoToCmd(CSceneUndoRedoCommand::EGroupChanged, UndoVar);

                        //1.复制出一样的一个组合图元(这里复制的是选择组合图元,之后应该删掉它)
                        CGraphicsItemGroup *pNewGroup = CDrawScene::copyCreatGroup(event->scene()->selectGroup());

                        QList<CGraphicsItem *> needSelectItems;
                        if (pNewGroup != nullptr) {
                            //2.获取到拷贝出来的图元
                            QList<CGraphicsItem *> outItems = pNewGroup->items();
                            QList<CGraphicsItem *> newBzItems = pNewGroup->getBzItems(true);

                            //3.删掉复制出来的临时用于选择管理的组合,即pNewGroup
                            event->scene()->destoryGroup(pNewGroup, true);
                            pNewGroup = nullptr;

                            //4.记录增加的基本业务图元(用于撤销还原)
                            QList<QVariant> vars;
                            vars << reinterpret_cast<long long>(event->scene());
                            for (auto p : newBzItems) {
                                vars << reinterpret_cast<long long>(p);
                            }
                            CUndoRedoCommand::recordUndoCommand(CUndoRedoCommand::ESceneChangedCmd,
                                                                CSceneUndoRedoCommand::EItemAdded, vars, false, true);

                            needSelectItems = outItems;
                        }

                        //5.复制完成后记录场景当前组合的快照(用于还原)这要在toolFinished再调用
                        //event->scene()->recordSecenInfoToCmd(CSceneUndoRedoCommand::EGroupChanged, RedoVar);

                        //6.选中被复制出来的图元
                        event->scene()->clearSelectGroup();
                        for (auto p : needSelectItems) {
                            event->scene()->selectItem(p, true, false, false);
                        }
                        event->scene()->selectGroup()->updateBoundingRect();
                        event->scene()->selectGroup()->updateAttributes();
                        pInfo->etcItems.append(event->scene()->selectGroup());
                    }
                } else {
                    tpye = EDragMove;
                    pInfo->etcItems.append(event->scene()->selectGroup());
                    QList<CGraphicsItem *> lists = event->scene()->selectGroup()->items(true);
                    event->scene()->recordItemsInfoToCmd(lists, UndoVar);
                }
                m_isItemMoving = true;
            } else if (event->scene()->isBussizeHandleNodeItem(pStartPosTopQtItem)) {
                CSizeHandleRect *pHandle = dynamic_cast<CSizeHandleRect *>(pStartPosTopQtItem);
                pInfo->_etcopeTpUpdate = pHandle->dir();
                pInfo->etcItems.clear();

                pInfo->etcItems.append(event->scene()->selectGroup());

                //记录undo点
                event->scene()->recordItemsInfoToCmd(event->scene()->selectGroup()->items(true), UndoVar, true);

                tpye = (pHandle->dir() != CSizeHandleRect::Rotation ? EResizeMove : ERotateMove);
            }
        }
    }
    return tpye;
}

void CSelectTool::mouseHoverEvent(IDrawTool::CDrawToolEvent *event)
{
    //处理高亮，鼠标样式变化等问题
    event->scene()->refreshLook(event->pos());
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

            QPen pen;
            pen.setWidthF(0.5);
            QBrush selectBrush = drawApp->systemThemeColor();
            QColor selectColor = selectBrush.color();
            selectColor.setAlpha(20);
            selectBrush.setColor(selectColor);
            selectColor.setAlpha(100);
            pen.setColor(selectColor);

            painter->setPen(pen);
            painter->setBrush(selectBrush);
            painter->drawRect(QRectF(topLeft, bomRight));
        } else if (info._opeTpUpdate == ERotateMove) {

            painter->setClipping(false);
            QPoint  posInView  = scene->drawView()->viewport()->mapFromGlobal(QCursor::pos());
            QPointF posInScene = scene->drawView()->mapToScene(posInView);

            qreal scled = scene->drawView()->getScale();
            QPointF paintPos = posInScene + QPointF(50 / scled, 0);

            qreal rote = scene->selectGroup()->count() == 0 ? 0 : scene->selectGroup()->items().first()->drawRotation();
            QString angle = QString("%1°").arg(QString::number(rote, 'f', 1));
            QFont f;
            f.setPointSizeF(11 / scled);

            QFontMetrics fontMetrics(f);
            int width = fontMetrics.width(angle);
            QRectF rotateRect(paintPos, paintPos + QPointF(width, fontMetrics.height()));

            painter->setPen(Qt::NoPen);
            painter->setBrush(QColor("#E5E5E5"));
            painter->drawRoundRect(rotateRect);
            painter->setFont(f);
            painter->setPen(Qt::black);
            painter->drawText(rotateRect, Qt::AlignCenter, angle);
            painter->setClipping(true);
        }
    }
    painter->restore();
}

bool CSelectTool::returnToSelectTool(CDrawToolEvent *event, ITERecordInfo *pInfo)
{
    Q_UNUSED(event)
    Q_UNUSED(pInfo)
    return false;
}

void CSelectTool::sendToolEventToItem(CDrawToolEvent *event,
                                      ITERecordInfo *info,
                                      EChangedPhase phase)
{
    CGraphItemEvent::EItemType tp = CGraphItemEvent::EUnKnow;
    switch (info->_opeTpUpdate) {
    case EDragMove:
    case ECopyMove:
        tp = CGraphItemEvent::EMove;
        break;
    case EResizeMove:
        tp = CGraphItemEvent::EScal;
        break;
    case ERotateMove:
        tp = CGraphItemEvent::ERot;
        break;
    default:
        tp = CGraphItemEvent::EUnKnow;
        break;
    }

    switch (tp) {
    case CGraphItemEvent::EMove: {
        processItemsMove(event, info, phase);
        break;
    }
    case CGraphItemEvent::EScal: {
        processItemsScal(event, info, phase);
        break;
    }
    case CGraphItemEvent::ERot: {
        processItemsRot(event, info, phase);
        break;
    }
    default:
        break;
    }
}

void CSelectTool::processItemsScal(IDrawTool::CDrawToolEvent *event, IDrawTool::ITERecordInfo *info, EChangedPhase phase)
{
    CGraphItemScalEvent scal(CGraphItemEvent::EScal);
    scal.setEventPhase(phase);
    scal.setToolEventType(info->_opeTpUpdate);
    scal.setPressedDirection(info->_etcopeTpUpdate);
    scal._scenePos = event->pos();
    scal._oldScenePos = info->_prePos;
    scal._sceneBeginPos = info->_startPos;

    bool xBlock = false;
    bool yBlock = false;
    CSizeHandleRect::EDirection dir = CSizeHandleRect::EDirection(info->_etcopeTpUpdate);
    CSizeHandleRect::getTransBlockFlag(dir, xBlock, yBlock);
    scal.setXTransBlocked(xBlock);
    scal.setYTransBlocked(yBlock);

    bool xNegitiveOffset = false;
    bool yNegitiveOffset = false;
    CSizeHandleRect::getTransNegtiveFlag(dir, xNegitiveOffset, yNegitiveOffset);
    scal.setXNegtiveOffset(xNegitiveOffset);
    scal.setYNegtiveOffset(yNegitiveOffset);
    scal.setKeepOrgRadio(event->keyboardModifiers() & Qt::ShiftModifier);

    //分发事件
    for (auto item : info->etcItems) {
        if (CDrawScene::isDrawItem(item) || item == event->scene()->selectGroup()) {
            CGraphicsItem *pBzItem = static_cast<CGraphicsItem *>(item);

            scal.setPos(pBzItem->mapFromScene(event->pos()));
            scal.setOldPos(pBzItem->mapFromScene(info->_prePos));
            scal.setOrgSize(pBzItem->rect().size());
            scal.setCenterPos((event->keyboardModifiers() & Qt::AltModifier) ? pBzItem->rect().center() :
                              CSizeHandleRect::transCenter(dir, pBzItem));
            scal._sceneCenterPos = pBzItem->mapToScene(scal.centerPos());

            if (phase == EChangedBegin) {
                pBzItem->operatingBegin(&scal);
            } else if (phase == EChangedUpdate) {
                pBzItem->operating(&scal);
            } else if (phase == EChangedFinished) {
                pBzItem->operatingEnd(&scal);
            }
        }
    }
}

void CSelectTool::processItemsRot(IDrawTool::CDrawToolEvent *event, IDrawTool::ITERecordInfo *info, EChangedPhase phase)
{
    CGraphItemRotEvent rot(CGraphItemEvent::ERot);
    rot.setEventPhase(phase);
    rot.setToolEventType(info->_opeTpUpdate);
    rot.setPressedDirection(info->_etcopeTpUpdate);
    rot._scenePos = event->pos();
    rot._oldScenePos = info->_prePos;
    rot._sceneBeginPos = info->_startPos;

    //分发事件
    for (auto item : info->etcItems) {
        if (CDrawScene::isDrawItem(item) || item == event->scene()->selectGroup()) {
            CGraphicsItem *pBzItem = static_cast<CGraphicsItem *>(item);
            rot.setPos(pBzItem->mapFromScene(event->pos()));
            rot.setOldPos(pBzItem->mapFromScene(info->_prePos));
            rot.setOrgSize(pBzItem->boundingRectTruly().size());
            rot.setCenterPos(pBzItem->boundingRectTruly().center());
            rot._sceneCenterPos = pBzItem->mapToScene(rot.centerPos());
            if (phase == EChangedBegin) {
                pBzItem->operatingBegin(&rot);
            } else if (phase == EChangedUpdate) {
                pBzItem->operating(&rot);
            } else if (phase == EChangedFinished) {
                pBzItem->operatingEnd(&rot);
            }
        }
    }
    event->view()->viewport()->update();
}

void CSelectTool::processItemsMove(CDrawToolEvent *event,
                                   ITERecordInfo *info,
                                   EChangedPhase phase)
{
    CGraphItemMoveEvent mov(CGraphItemEvent::EMove);
    mov.setEventPhase(phase);
    mov.setToolEventType(info->_opeTpUpdate);
    mov.setPressedDirection(info->_etcopeTpUpdate);
    mov._scenePos = event->pos();
    mov._oldScenePos = info->_prePos;
    mov._sceneBeginPos = info->_startPos;

    //分发事件
    for (auto item : info->etcItems) {
        if (CDrawScene::isDrawItem(item) || item == event->scene()->selectGroup()) {
            CGraphicsItem *pBzItem = static_cast<CGraphicsItem *>(item);
            mov.setPos(pBzItem->mapFromScene(event->pos()));
            mov.setOldPos(pBzItem->mapFromScene(info->_prePos));
            mov.setOrgSize(pBzItem->boundingRectTruly().size());
            mov.setCenterPos(pBzItem->boundingRectTruly().center());
            mov._sceneCenterPos = pBzItem->mapToScene(mov.centerPos());
            if (phase == EChangedBegin) {
                pBzItem->operatingBegin(&mov);
            } else if (phase == EChangedUpdate) {
                pBzItem->operating(&mov);
            } else if (phase == EChangedFinished) {
                pBzItem->operatingEnd(&mov);
            }
        }
    }
}
