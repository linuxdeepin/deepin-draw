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

#include "service/cmanagerattributeservice.h"

#include <DApplication>
#include <QScrollBar>
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
    , m_isItemMoving(false)
{
}

CSelectTool::~CSelectTool()
{

}

//bool CSelectTool::isDragging()
//{
//    return m_isItemMoving;
//}

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
    IDrawTool::mouseDoubleClickEvent(event, scene);
}

void CSelectTool::toolStart(IDrawTool::CDrawToolEvent *event, ITERecordInfo *pInfo)
{
    _hightLight = QPainterPath();

    QGraphicsItem *pStartPostTopBzItem = pInfo->startPosTopBzItem;

    QGraphicsItem *pFirstItem = pInfo->startPosItems.isEmpty() ? nullptr : pInfo->startPosItems.first();
    bool isMrNodeItem = event->scene()->isBussizeHandleNodeItem(pFirstItem) && (event->scene()->getAssociatedBzItem(pFirstItem)->type() == MgrType);

    bool doSelect = true;
    bool clearBeforeSelect = true;

    if (event->keyboardModifiers() == Qt::ShiftModifier) {
        // 点住shift那么不用清除
        clearBeforeSelect = false;

        if (!isMrNodeItem && pStartPostTopBzItem != nullptr && pStartPostTopBzItem->isSelected()) {
            if (event->scene()->getItemsMgr()->count() > 1) {
                event->scene()->notSelectItem(pStartPostTopBzItem);
                //event->setAccepted(true);
                return;
            }
        }
    } else {
        if (isMrNodeItem) {
            clearBeforeSelect = false;
        } else if (pStartPostTopBzItem != nullptr) {
            if (pStartPostTopBzItem->isSelected()) {
                //点击的是当前选中了的相等的图元， 也不用清除当前选中
                clearBeforeSelect = false;
            }
        }
    }

    //清理当前选中
    if (clearBeforeSelect) {
        event->scene()->clearMrSelection();
    }

    if (doSelect) {
        if (pStartPostTopBzItem != nullptr) {
            if (!isMrNodeItem)
                event->scene()->selectItem(pStartPostTopBzItem);
            //event->setAccepted(true);
        } else {
            if (!isMrNodeItem) {
                //点击处是空白的那么清理所有选中

                event->scene()->clearMrSelection();
            }
            //event->setAccepted(true);
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
            qWarning() << "EResizeMove operating but CSizeHandleRect::EDirection is CSizeHandleRect::Rotation,so do nothing!";
        }
        break;
    }
    case ERotateMove: {
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
            event->view()->viewport()->update();

            drawApp->setApplicationCursor(pMrItem->handleNode()->getCursor());
        }
        break;
    }
    case ECopyMove: {
        //复制移动
        if (!pInfo->etcItems.isEmpty()) {
            QPointF move = event->pos() - pInfo->_prePos;
            event->scene()->moveItems(pInfo->etcItems, move);
        }
        event->scene()->getItemsMgr()->updateBoundingRect();
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
        event->scene()->recordItemsInfoToCmd(event->scene()->getItemsMgr()->getItems(), false);
        m_isItemMoving = false;
        break;
    }
    case EResizeMove: {
        //记录Redo点
        event->scene()->recordItemsInfoToCmd(event->scene()->getItemsMgr()->getItems(), false);
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
        m_isItemMoving = false;
        break;
    }
    case ERotateMove: {
        break;
    }
    default:
        break;
    }

    QList<CGraphicsItem *> items = event->scene()->getItemsMgr()->getItems();
    if (!items.isEmpty()) {
        QGraphicsItem *pItem = items.first();
        if (pItem != nullptr && pItem->type() == PictureType) {
            event->scene()->getItemsMgr()->updateAttributes();
        }
    }

    //入栈
    CUndoRedoCommand::finishRecord();

    IDrawTool::toolFinish(event, pInfo);
}

void CSelectTool::toolDoubleClikedEvent(IDrawTool::CDrawToolEvent *event, IDrawTool::ITERecordInfo *pInfo)
{
    IDrawTool::toolDoubleClikedEvent(event, pInfo);
    //qDebug() << "pInfo->startPosTopBzItem = " << pInfo->startPosTopBzItem << "is mrg = " << (pInfo->startPosTopBzItem != nullptr ? pInfo->startPosTopBzItem->type() == MgrType : false);
    if (pInfo->startPosTopBzItem != nullptr) {
        //qDebug() << "pInfo->startPosTopBzItem type = " << pInfo->startPosTopBzItem->type();
        if (pInfo->startPosTopBzItem->type() == TextType) {
            CGraphicsTextItem *pTextItem = dynamic_cast<CGraphicsTextItem *>(pInfo->startPosTopBzItem);
            pTextItem->makeEditabel();
        }
    }
    event->setAccepted(true);
}

int CSelectTool::decideUpdate(IDrawTool::CDrawToolEvent *event, IDrawTool::ITERecordInfo *pInfo)
{
    int tpye = ENothingDo;
    if (isActived()) {
        QGraphicsItem *pStartPosTopQtItem = event->scene()->firstItem(pInfo->_startPos,
                                                                      pInfo->startPosItems, true, true);
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
                    pInfo->etcItems = copyItemsToScene(event->scene()->getBzItems(event->scene()->selectedItems()),
                                                       event->scene());

                    event->scene()->clearMrSelection();

                    //记录还原点，之后入栈
                    QList<QVariant> vars;
                    vars << reinterpret_cast<long long>(event->scene());
                    for (int i = 0; i < pInfo->etcItems.size(); ++i) {
                        QGraphicsItem *pItem = pInfo->etcItems[i];
                        event->scene()->selectItem(pItem);
                        vars << reinterpret_cast<long long>(pItem);
                    }
                    CUndoRedoCommand::recordUndoCommand(CUndoRedoCommand::ESceneChangedCmd,
                                                        CSceneUndoRedoCommand::EItemAdded, vars, true, true);
                } else {
                    tpye = EDragMove;
                    pInfo->etcItems.append(event->scene()->getItemsMgr());
                    QList<CGraphicsItem *> lists = event->scene()->getItemsMgr()->getItems();
                    event->scene()->recordItemsInfoToCmd(lists, true);
                }
                m_isItemMoving = true;
            } else if (event->scene()->isBussizeHandleNodeItem(pStartPosTopQtItem)) {
                CSizeHandleRect *pHandle = dynamic_cast<CSizeHandleRect *>(pStartPosTopQtItem);
                pInfo->_etcopeTpUpdate = pHandle->dir();
                pInfo->etcItems.clear();

                pInfo->etcItems.append(event->scene()->getItemsMgr());

                //记录undo点
                event->scene()->recordItemsInfoToCmd(event->scene()->getItemsMgr()->getItems(), true);

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
        } else if (info._opeTpUpdate == ERotateMove) {

            painter->setClipping(false);
            QPoint  posInView  = scene->drawView()->viewport()->mapFromGlobal(QCursor::pos());
            QPointF posInScene = scene->drawView()->mapToScene(posInView);

            qreal scled = scene->drawView()->getScale();
            QPointF paintPos = posInScene + QPointF(50 / scled, 0);

            QString angle = QString("%1°").arg(QString::number(scene->getItemsMgr()->rotation(), 'f', 1));
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
            //scene->drawView()->viewport()->update();
            painter->setClipping(true);
        }
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
    //    CGraphicsView *view = CManageViewSigleton::GetInstance()->getCurView();
    //    if (view != nullptr) {
    //        if (view != nullptr && view->scene() != nullptr) {

    //            if (view->isKeySpacePressed()) {
    //                return ;
    //            }

    //            QPoint viewPortPos =  view->viewport()->mapFromGlobal(QCursor::pos());
    //            QPointF scenePos = view->mapToScene(viewPortPos);
    //            QList<QGraphicsItem *> posItems = view->scene()->items(scenePos);
    //            if (posItems.isEmpty()) {
    //                dApp->setApplicationCursor(Qt::ArrowCursor);
    //            } else {
    //                QGraphicsItem *pFirstItem = posItems.first();

    //                //CSizeHandleRect的父类QGraphicsSvgItem的类型就是13
    //                if (pFirstItem->type() == QGraphicsSvgItem::Type) {
    //                    CSizeHandleRect *pHandleItem = dynamic_cast<CSizeHandleRect *>(pFirstItem);
    //                    if (pHandleItem != nullptr) {
    //                        dApp->setApplicationCursor(getCursor(pHandleItem->dir(), false));
    //                    } else {
    //                        dApp->setApplicationCursor(Qt::ArrowCursor);
    //                    }
    //                } else if (pFirstItem->type() == QGraphicsProxyWidget::Type) {
    //                    //QGraphicsProxyWidget的类型就是12
    //                    QGraphicsProxyWidget *pProxyWidget = dynamic_cast<QGraphicsProxyWidget *>(pFirstItem);
    //                    if (pProxyWidget != nullptr) {
    //                        CGraphicsTextItem *pTextItem = dynamic_cast<CGraphicsTextItem *>(pProxyWidget->parentItem());
    //                        if (pTextItem != nullptr && pTextItem->isEditable()) {
    //                            dApp->setApplicationCursor(m_textEditCursor);
    //                        } else {
    //                            dApp->setApplicationCursor(Qt::ArrowCursor);
    //                        }
    //                    } else {
    //                        dApp->setApplicationCursor(Qt::ArrowCursor);
    //                    }

    //                } else {
    //                    dApp->setApplicationCursor(Qt::ArrowCursor);
    //                }
    //            }
    //        }
    //    }
}

bool CSelectTool::returnToSelectTool(CDrawToolEvent *event, ITERecordInfo *pInfo)
{
    Q_UNUSED(event)
    Q_UNUSED(pInfo)
    return false;
}
