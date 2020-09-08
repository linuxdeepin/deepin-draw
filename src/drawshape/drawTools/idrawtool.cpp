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
#include "idrawtool.h"
#include "cdrawscene.h"
#include "frame/cviewmanagement.h"
#include "frame/cgraphicsview.h"
#include "drawshape/cdrawparamsigleton.h"
#include "application.h"
#include "frame/cundoredocommand.h"
#include "cgraphicsitem.h"
#include <QDebug>
#include <QKeyEvent>
#include <QGraphicsSceneEvent>
#include <QGraphicsProxyWidget>
#include <QTimer>

QTimer *IDrawTool::s_timerForDoubleClike = nullptr;

IDrawTool::~IDrawTool()
{

}

IDrawTool::IDrawTool(EDrawToolMode mode)
    : m_bMousePress(false)
    , m_sPointPress(0, 0)
    , m_sLastPress(0, 0)
    , m_sPointRelease(0, 0)
    , m_bShiftKeyPress(false)
    , m_bAltKeyPress(false)
    , m_noShiftSelectItem(nullptr)
    , m_mode(mode)
    , m_RotateCursor(QPixmap(":/theme/light/images/mouse_style/rotate_mouse.svg").scaled(24, 24, Qt::IgnoreAspectRatio, Qt::SmoothTransformation))
    , m_LeftTopCursor(QPixmap(":/theme/light/images/mouse_style/icon_drag_leftup.svg").scaled(24, 24, Qt::IgnoreAspectRatio, Qt::SmoothTransformation))
    , m_RightTopCursor(QPixmap(":/theme/light/images/mouse_style/icon_drag_rightup.svg").scaled(24, 24, Qt::IgnoreAspectRatio, Qt::SmoothTransformation))
    , m_LeftRightCursor(QPixmap(":/theme/light/images/mouse_style/icon_drag_left.svg").scaled(24, 24, Qt::IgnoreAspectRatio, Qt::SmoothTransformation))
    , m_UpDownCursor(QPixmap(":/theme/light/images/mouse_style/icon_drag_up.svg").scaled(24, 24, Qt::IgnoreAspectRatio, Qt::SmoothTransformation))
{

}
void IDrawTool::mousePressEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene)
{
    //1.如果由qt将触控事件转成的鼠标事件那么不要再调用toolDoStart（因为在scene的event中已经处理过）
    if (event->source() == Qt::MouseEventSynthesizedByQt) {
        return scene->mouseEvent(event);
    }

    CDrawToolEvent::CDrawToolEvents e = CDrawToolEvent::fromQEvent(event, scene);

    toolDoStart(&e.first());

    if (!e.first().isAccepted()) {
        scene->mouseEvent(event);
    }
}

void IDrawTool::mouseMoveEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene)
{
    //1.如果由qt将触控事件转成的鼠标事件那么不要再调用toolDoUpdate（因为在scene的event中已经处理过）
    if (event->source() == Qt::MouseEventSynthesizedByQt) {
        return scene->mouseEvent(event);
    }

    CDrawToolEvent::CDrawToolEvents e = CDrawToolEvent::fromQEvent(event, scene);

    toolDoUpdate(&e.first());

    //已经处理完成，不用传递到框架,否则传递给框架继续处理
    if (!e.first().isAccepted()) {
        scene->mouseEvent(event);
    }
}

void IDrawTool::mouseReleaseEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene)
{
    //1.如果由qt将触控事件转成的鼠标事件那么不要再调用toolDoFinish（因为在scene的event中已经处理过）
    if (event->source() == Qt::MouseEventSynthesizedByQt) {
        return scene->mouseEvent(event);
    }

    CDrawToolEvent::CDrawToolEvents e = CDrawToolEvent::fromQEvent(event, scene);

    toolDoFinish(&e.first());

    //已经处理完成，不用传递到框架,否则传递给框架继续处理
    if (!e.first().isAccepted()) {
        scene->mouseEvent(event);
    }
}
void IDrawTool::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene)
{
    //1.如果由qt将触控事件转成的鼠标事件那么不要再调用（因为在scene的event中已经处理过）
    if (event->source() == Qt::MouseEventSynthesizedByQt) {
        return scene->mouseEvent(event);
    }

    CDrawToolEvent::CDrawToolEvents e = CDrawToolEvent::fromQEvent(event, scene);

    dueTouchDoubleClickedStart(&e.first());

    if (!e.first().isAccepted()) {
        scene->mouseEvent(event);
    }
}

bool IDrawTool::isActived()
{
    return !_allITERecordInfo.isEmpty();
}

void IDrawTool::interrupt()
{
    clearITE();
}

//int IDrawTool::activedType()
//{
//    if (!_allITERecordInfo.isEmpty()) {
//        return _allITERecordInfo.last()._opeTpUpdate;
//    }
//    return -1;
//}

void IDrawTool::toolDoStart(IDrawTool::CDrawToolEvent *event)
{
    event->scene()->clearHighlight();

    if (event->mouseButtons() == Qt::LeftButton || event->eventType() == CDrawToolEvent::ETouchEvent) {

        //m_bMousePress = true;

        if (dueTouchDoubleClickedStart(event)) {
            return;
        }

        ITERecordInfo info;

        info._prePos       = event->pos();
        info._startPos     = event->pos();
        info.startPosItems = event->scene()->items(event->pos());
        info.startPosTopBzItem = event->scene()->topBzItem(event->pos(), true);
        info._isvaild  = true;
        info._curEvent = *event;
        info._startEvent = *event;
        info._scene    = event->scene();
        info.getTimeHandle()->restart();

        if (getCurVaildActivedPointCount() >= allowedMaxTouchPointCount()) {
            event->setAccepted(true);
            //超过可支持的点数了
            info.eventLife = EDoNotthing;
            qDebug() << "toolDoStart rInfo.eventLife = EDoNotthing id " << event->uuid();
            _allITERecordInfo.insert(event->uuid(), info);
        } else if (isPressEventHandledByQt(event, &info)) {
            event->setAccepted(false);
            info.eventLife = EDoQtCoversion;
            qDebug() << "toolDoStart rInfo.eventLife = EDoQtCoversion id " << event->uuid();
            //只有真实的鼠标事件的调用才需要备份该事件信息,之后会调用toolDoUpdate,toolDoFinished，从而在其中通过UUID找到该事件信息以决定再将该事件传递给mouseEvent
            //触控操作在这里会被转成qt的鼠标事件直接去调用mouseEvent，不会再进入toolDoStart，toolDoUpdate,toolDoFinished，所以不需要备份保存信息
            if (event->eventType() == CDrawToolEvent::EMouseEvent)
                _allITERecordInfo.insert(event->uuid(), info);
        } else {
            info.businessItem  = creatItem(event);
            info.eventLife     = ENormal;
            qDebug() << "toolDoStart rInfo.eventLife = ENormal id " << event->uuid();

            _allITERecordInfo.insert(event->uuid(), info);

            if (info.businessItem != nullptr) {

                //工具开始创建图元应该清理当前选中情况
                event->scene()->clearSelection();

                toolCreatItemStart(event, &info);

            } else {
                toolStart(event, &info);
            }
        }
    }
}

void IDrawTool::toolDoUpdate(IDrawTool::CDrawToolEvent *event)
{
    if (!_allITERecordInfo.isEmpty()) {
        auto it = _allITERecordInfo.find(event->uuid());
        if (it != _allITERecordInfo.end()) {
            ITERecordInfo &rInfo = it.value();
            rInfo._preEvent = rInfo._curEvent;
            rInfo._curEvent = *event;

            //判定是否移动过(根据工具不同移动的最小距离值不同可重载minMoveUpdateDistance)
            if (!rInfo.hasMoved()) {
                int constDis = minMoveUpdateDistance();

                QPointF offset = event->pos(CDrawToolEvent::EViewportPos) -
                                 rInfo._startEvent.pos(CDrawToolEvent::EViewportPos)/*event->view()->mapFromScene(rInfo._startPos)*/;
                int curDis = qRound(offset.manhattanLength());
                rInfo._moved = (curDis >= constDis);
            }

            if (rInfo.eventLife == EDoNotthing) {
                event->setAccepted(true);
            } else if (rInfo.eventLife == EDoQtCoversion) {
                //0.如果开始事件是不被接受的（传递给了Qt框架），那么Update也应该不被接受（也应该传递给Qt框架）
                event->setAccepted(false);
            } else {
                if (!rInfo.haveDecidedOperateType) {
                    //判定应该做什么
                    if (rInfo.businessItem != nullptr) {
                        if (rInfo.hasMoved()) {
                            rInfo._opeTpUpdate = EToolCreatItemMove;
                            rInfo.haveDecidedOperateType = true;
                        }
                    } else {
                        QRectF rectf(event->view()->mapFromScene(rInfo._startPos) - QPointF(10, 10), QSizeF(20, 20));
                        if (!rectf.contains(event->pos(CDrawToolEvent::EViewportPos))) {
                            QTime *elTi = rInfo.getTimeHandle();
                            rInfo._elapsedToUpdate = (elTi == nullptr ? -1 : elTi->elapsed());
                            rInfo._opeTpUpdate = decideUpdate(event, &rInfo);
                            //调用图元的operatingBegin函数
                            if (rInfo._opeTpUpdate > EToolDoNothing) {
                                for (auto it : rInfo.etcItems) {
                                    if (event->scene()->isBussizeItem(it) || it->type() == MgrType) {
                                        CGraphicsItem *pBzItem = dynamic_cast<CGraphicsItem *>(it);
                                        pBzItem->operatingBegin(rInfo._opeTpUpdate);
                                    }
                                }
                            }
                            rInfo.haveDecidedOperateType = true;
                        }
                    }
                }
                //2.执行操作
                if (rInfo.haveDecidedOperateType) {
                    if (rInfo._opeTpUpdate == EToolCreatItemMove) {
                        toolCreatItemUpdate(event, &rInfo);
                    } else if (rInfo._opeTpUpdate > EToolDoNothing) {
                        toolUpdate(event, &rInfo);
                    }
                }
            }
            rInfo._prePos = event->pos();
        }
    } else {
        mouseHoverEvent(event);
    }
}

void IDrawTool::toolDoFinish(IDrawTool::CDrawToolEvent *event)
{
    bool updateCursor = true;
    qDebug() << "toolDoFinish ==== " << event->uuid();
    if (!_allITERecordInfo.isEmpty()) {
        auto it = _allITERecordInfo.find(event->uuid());
        if (it != _allITERecordInfo.end()) {
            ITERecordInfo &rInfo = it.value();
            rInfo._prePos = event->pos();
            rInfo._preEvent = rInfo._curEvent;
            rInfo._curEvent = *event;
            qDebug() << "toolDoFinish rInfo.eventLife = " << rInfo.eventLife;

            CGraphicsItem *pCreatedItem = nullptr;
            if (rInfo.eventLife == EDoNotthing) {
                event->setAccepted(true);
            } else if (rInfo.eventLife == EDoQtCoversion) {
                event->setAccepted(false);
            } else {
                //1.根据操作类型决定要做的事情
                if (rInfo.businessItem != nullptr) {
                    toolCreatItemFinish(event, &rInfo);
                    pCreatedItem = rInfo.businessItem;
                    if (rInfo.businessItem != nullptr) {
                        if (rInfo.businessItem->scene() == event->scene()) {
                            CCmdBlock block(event->scene(), CSceneUndoRedoCommand::EItemAdded, rInfo.businessItem);
                            //event->scene()->selectItem(rInfo.businessItem);
                        }
                    }
                } else if (rInfo._opeTpUpdate > EToolDoNothing) {
                    for (auto it : rInfo.etcItems) {
                        if (event->scene()->isBussizeItem(it) || it->type() == MgrType) {
                            CGraphicsItem *pBzItem = dynamic_cast<CGraphicsItem *>(it);
                            pBzItem->operatingEnd(rInfo._opeTpUpdate);
                        }
                    }
                    toolFinish(event, &rInfo);
                }
                // 保证恢复到正常绘制
                if (!event->view()->isPaintEnable()) {
                    event->view()->setPaintEnable(true);
                }
            }

            qDebug() << "finished uuid ===== " << event->uuid() << "allITERecordInfo count = " << _allITERecordInfo.count();

            //2.是否要回到select工具模式下去(多个触控时即将为空才判断)
            if (_allITERecordInfo.count() == 1 && returnToSelectTool(event, &rInfo)) {
                if (pCreatedItem != nullptr) {
                    event->scene()->selectItem(pCreatedItem);
                }
                setViewToSelectionTool();
            } else {
                //不回到select工具时不用刷新鼠标样式（维持工具的鼠标样式方便提示用户可继续绘制）
                updateCursor = false;
            }

            _allITERecordInfo.erase(it);
        }
    } else {
        event->setAccepted(false);
    }
    if (event->eventType() == CDrawToolEvent::EMouseEvent && updateCursor)
        event->scene()->refreshLook(event->pos());
}

bool IDrawTool::dueTouchDoubleClickedStart(IDrawTool::CDrawToolEvent *event)
{
    static int intervalMs = 250;
    if (event->eventType() == CDrawToolEvent::ETouchEvent) {
        static QPointF prePos;
        //touch触控判定是否是双击
        if (!getTimerForDoubleCliked()->isActive()) {
            prePos = event->pos();
            getTimerForDoubleCliked()->start(intervalMs);
        } else {
            //判定移动的幅度很小
            QRectF rectf(prePos - QPointF(10, 10), QSizeF(20, 20));
            if (rectf.contains(event->pos())) {
                m_bMousePress = true;

                ITERecordInfo info;

                info._prePos = event->pos();
                info._startPos = event->pos();
                info.startPosItems = event->scene()->items(event->pos());
                info.startPosTopBzItem = event->scene()->topBzItem(event->pos());
                info._isvaild = true;
                info._curEvent = *event;
                info._scene = event->scene();

                _allITERecordInfo.insert(event->uuid(), info);

                toolDoubleClikedEvent(event, &info);

                getTimerForDoubleCliked()->stop();

                return true;
            }
        }
    } else if (event->eventType() == CDrawToolEvent::EMouseEvent) {
        m_bMousePress = true;
        QEvent::Type tp = event->orgQtEvent()->type();
        if (tp == QEvent::MouseButtonDblClick || tp == QEvent::GraphicsSceneMouseDoubleClick) {
            ITERecordInfo info;

            info._prePos = event->pos();
            info._startPos = event->pos();
            info.startPosItems = event->scene()->items(event->pos());
            info.startPosTopBzItem = event->scene()->topBzItem(event->pos());
            info._isvaild = true;
            info._curEvent = *event;
            info._scene = event->scene();

            _allITERecordInfo.insert(event->uuid(), info);

            toolDoubleClikedEvent(event, &info);

            return true;
        }
    }
    return false;
}

void IDrawTool::toolStart(IDrawTool::CDrawToolEvent *event, ITERecordInfo *pInfo)
{
    Q_UNUSED(pInfo)

    //默认不传递事件给框架
    event->setAccepted(true);
}

void IDrawTool::toolUpdate(IDrawTool::CDrawToolEvent *event, ITERecordInfo *pInfo)
{
    Q_UNUSED(pInfo)

    //默认传递事件给框架
    event->setAccepted(true);
}

void IDrawTool::toolFinish(IDrawTool::CDrawToolEvent *event, ITERecordInfo *pInfo)
{
    Q_UNUSED(pInfo)

    //默认传递事件给框架
    event->setAccepted(true);

    event->scene()->update();
}

int IDrawTool::decideUpdate(IDrawTool::CDrawToolEvent *event, IDrawTool::ITERecordInfo *pInfo)
{
    Q_UNUSED(event)
    Q_UNUSED(pInfo)

    //默认-2是 刷新新创造的item的路径 的标记（只是必要条件 不充分）
    return -2;
}

void IDrawTool::toolCreatItemStart(IDrawTool::CDrawToolEvent *event, IDrawTool::ITERecordInfo *pInfo)
{
    Q_UNUSED(pInfo)

    //默认不传递事件给框架
    event->setAccepted(true);

    event->scene()->update();
}

void IDrawTool::toolCreatItemUpdate(IDrawTool::CDrawToolEvent *event, IDrawTool::ITERecordInfo *pInfo)
{
    Q_UNUSED(pInfo)

    //默认不传递事件给框架
    event->setAccepted(true);

    event->scene()->update();
}

void IDrawTool::toolCreatItemFinish(IDrawTool::CDrawToolEvent *event, IDrawTool::ITERecordInfo *pInfo)
{
    Q_UNUSED(pInfo)

    //默认不传递事件给框架
    event->setAccepted(true);

    event->scene()->update();
}

void IDrawTool::clearITE()
{
    _allITERecordInfo.clear();
}

void IDrawTool::setViewToSelectionTool(CGraphicsView *pView)
{
    if (pView == nullptr) {
        pView = CManageViewSigleton::GetInstance()->getCurView();
    }

    if (pView != nullptr) {
        pView->getDrawParam()->setCurrentDrawToolMode(selection);
        emit pView->drawScene()->signalChangeToSelect();
    }
}

CGraphicsItem *IDrawTool::creatItem(CDrawToolEvent *event)
{
    Q_UNUSED(event)
    return nullptr;
}

int IDrawTool::minMoveUpdateDistance()
{
    return dApp->startDragDistance();
}

void IDrawTool::mouseHoverEvent(IDrawTool::CDrawToolEvent *event)
{
    // 只有鼠标才存在hover事件
    Q_UNUSED(event)
}

void IDrawTool::toolDoubleClikedEvent(IDrawTool::CDrawToolEvent *event, ITERecordInfo *pInfo)
{
    Q_UNUSED(event)
    Q_UNUSED(pInfo)
    qDebug() << "IDrawTool::toolDoubleClikedEvent == " << event->pos() << "tp = " << event->eventType();
}

void IDrawTool::drawMore(QPainter *painter, const QRectF &rect, CDrawScene *scene)
{
    //注意painter是在viewport的坐标系,绘制时需要转换

    Q_UNUSED(painter)
    Q_UNUSED(rect)
    Q_UNUSED(scene)
}

EDrawToolMode IDrawTool::getDrawToolMode() const
{
    return m_mode;
}

QCursor IDrawTool::getCursor(CSizeHandleRect::EDirection dir, bool bMouseLeftPress, char toolType)
{
    CGraphicsView *pView = CManageViewSigleton::GetInstance()->getCurView();
    if (pView != nullptr) {
        if (pView->isKeySpacePressed()) {
            return (*qApp->overrideCursor());
        }
    }

    Qt::CursorShape result;
    QCursor resultCursor;
    QMatrix matrix;
    QPixmap pixmap;
    switch (dir) {
    case CSizeHandleRect::Right:
        if (bMouseLeftPress) {
            result =  Qt::ClosedHandCursor;
            resultCursor = QCursor(result);
        } else {
            resultCursor = m_LeftRightCursor;
            matrix.rotate(getCursorRotation());
            pixmap = resultCursor.pixmap().transformed(matrix, Qt::SmoothTransformation);
            resultCursor = QCursor(pixmap);
        }
        break;
    case CSizeHandleRect::RightTop:
        if (bMouseLeftPress) {
            result =  Qt::ClosedHandCursor;
            resultCursor = QCursor(result);
        } else {
            resultCursor = m_RightTopCursor;
            matrix.rotate(getCursorRotation());
            pixmap = resultCursor.pixmap().transformed(matrix, Qt::SmoothTransformation);
            resultCursor = QCursor(pixmap);
        }
        break;
    case CSizeHandleRect::RightBottom:
        if (bMouseLeftPress) {
            result =  Qt::ClosedHandCursor;
            resultCursor = QCursor(result);
        } else {
            resultCursor = m_LeftTopCursor;
            matrix.rotate(getCursorRotation());
            pixmap = resultCursor.pixmap().transformed(matrix, Qt::SmoothTransformation);
            resultCursor = QCursor(pixmap);
        }
        break;
    case CSizeHandleRect::LeftBottom:
        if (bMouseLeftPress) {
            result =  Qt::ClosedHandCursor;
            resultCursor = QCursor(result);
        } else {
            resultCursor = m_RightTopCursor;
            matrix.rotate(getCursorRotation());
            pixmap = resultCursor.pixmap().transformed(matrix, Qt::SmoothTransformation);
            resultCursor = QCursor(pixmap);
        }
        break;
    case CSizeHandleRect::Bottom:
        if (bMouseLeftPress) {
            result =  Qt::ClosedHandCursor;
            resultCursor = QCursor(result);
        } else {
            resultCursor = m_UpDownCursor;
            matrix.rotate(getCursorRotation());
            pixmap = resultCursor.pixmap().transformed(matrix, Qt::SmoothTransformation);
            resultCursor = QCursor(pixmap);
        }
        break;
    case CSizeHandleRect::LeftTop:
        if (bMouseLeftPress) {
            result =  Qt::ClosedHandCursor;
            resultCursor = QCursor(result);
        } else {
            resultCursor = m_LeftTopCursor;
            matrix.rotate(getCursorRotation());
            pixmap = resultCursor.pixmap().transformed(matrix, Qt::SmoothTransformation);
            resultCursor = QCursor(pixmap);
        }
        break;
    case CSizeHandleRect::Left:
        if (bMouseLeftPress) {
            result =  Qt::ClosedHandCursor;
            resultCursor = QCursor(result);
        } else {
            resultCursor = m_LeftRightCursor;
            matrix.rotate(getCursorRotation());
            pixmap = resultCursor.pixmap().transformed(matrix, Qt::SmoothTransformation);
            resultCursor = QCursor(pixmap);
        }
        break;
    case CSizeHandleRect::Top:
        if (bMouseLeftPress) {
            result =  Qt::ClosedHandCursor;
            resultCursor = QCursor(result);
        } else {
            resultCursor = m_UpDownCursor;
            matrix.rotate(getCursorRotation());
            pixmap = resultCursor.pixmap().transformed(matrix, Qt::SmoothTransformation);
            resultCursor = QCursor(pixmap);
        }
        break;

    case CSizeHandleRect::Rotation: {
        resultCursor = m_RotateCursor;
        matrix.rotate(getCursorRotation());
        pixmap = resultCursor.pixmap().transformed(matrix, Qt::SmoothTransformation);
        resultCursor = QCursor(pixmap);
    }
    break;
    case CSizeHandleRect::InRect:
        if (toolType == 0) {
            if (bMouseLeftPress) {
                result =  Qt::ClosedHandCursor;
            } else {
                result =  Qt::OpenHandCursor;
            }
            resultCursor = QCursor(result);
        }
        break;
    case CSizeHandleRect::None:
        result =  Qt::ArrowCursor;
        resultCursor = QCursor(result);
        break;
//    default:
//        result =  Qt::ArrowCursor;
//        resultCursor = QCursor(result);
//        break;
    }

    return resultCursor;
}

qreal IDrawTool::getCursorRotation()
{
    qreal angle = 0;
    CGraphicsView *pView = CManageViewSigleton::GetInstance()->getCurView();
    if (pView != nullptr) {
        CDrawScene *scene = static_cast<CDrawScene *>(pView->scene());
        QList<QGraphicsItem *> allSelectItems = scene->selectedItems();
        for (int i = allSelectItems.size() - 1; i >= 0; i--) {
            if (allSelectItems.at(i)->zValue() == 0.0) {
                allSelectItems.removeAt(i);
                continue;
            }
            if (allSelectItems[i]->type() <= QGraphicsItem::UserType || allSelectItems[i]->type() >= EGraphicUserType::MgrType) {
                allSelectItems.removeAt(i);
            }
        }

        if (allSelectItems.size() >= 1) {
            angle = allSelectItems.at(0)->rotation();
        }
    }
    return  angle;
}

int IDrawTool::allowedMaxTouchPointCount()
{
    return 1;
}

bool IDrawTool::returnToSelectTool(CDrawToolEvent *event, ITERecordInfo *pInfo)
{
    Q_UNUSED(event)
    Q_UNUSED(pInfo)
    return true;
}

int IDrawTool::getCurVaildActivedPointCount()
{
    int result = 0;
    for (auto it = _allITERecordInfo.begin(); it != _allITERecordInfo.end(); ++it) {
        if (it.value().eventLife != EDoNotthing) {
            ++result;
        }
    }
    return result;
}

bool IDrawTool::isPressEventHandledByQt(IDrawTool::CDrawToolEvent *event, IDrawTool::ITERecordInfo *pInfo)
{
    Q_UNUSED(event)
    Q_UNUSED(pInfo)
    //1.节点node不用传递给qgraphics的qt框架 自己处理调整图元大小
    QGraphicsItem *pItem = pInfo->startPosItems.isEmpty() ? nullptr : pInfo->startPosItems.first();
    if (event->scene()->isBussizeHandleNodeItem(pItem)) {
        return false;
    }

    //2.如果不是节点，那么如果是代理的widget那么传递给qgraphics的qt框架，将事件传递给这个widget

    QGraphicsItem *pFocusItem = event->scene()->focusItem();
    bool b = (pFocusItem != nullptr &&
              pFocusItem->type() == QGraphicsProxyWidget::Type);
    //qDebug() << "event->scene()->focusItem() = " << event->scene()->focusItem() << "event->scene()->focusItem()->type() = " << event->scene()->focusItem()->type() << "b = " << b;
    return b;
}

//IDrawTool::ITERecordInfo *IDrawTool::getEventIteInfo(int uuid)
//{
//    auto itf = _allITERecordInfo.find(uuid);
//    if (itf != _allITERecordInfo.end()) {
//        return &itf.value();
//    }
//    return nullptr;
//}

QTimer *IDrawTool::getTimerForDoubleCliked()
{
    if (s_timerForDoubleClike == nullptr) {
        s_timerForDoubleClike = new QTimer;
        s_timerForDoubleClike->setSingleShot(true);
    }
    return s_timerForDoubleClike;
}

IDrawTool::CDrawToolEvent::CDrawToolEvent(const QPointF &vPos,
                                          const QPointF &scenePos,
                                          const QPointF &globelPos,
                                          CDrawScene *pScene)
{
    _pos[EViewportPos] = vPos;
    _pos[EScenePos]    = scenePos;
    _pos[EGlobelPos]   = globelPos;
    _scene             = pScene;
}

IDrawTool::CDrawToolEvent::CDrawToolEvents IDrawTool::CDrawToolEvent::fromQEvent(QEvent *event, CDrawScene *scene)
{
    CDrawToolEvents eList;
    CDrawToolEvent e;
    switch (event->type()) {
    case QEvent::MouseButtonPress:
    case QEvent::MouseButtonRelease:
    case QEvent::MouseMove:
    case QEvent::MouseButtonDblClick: {
        QMouseEvent *msEvent = dynamic_cast<QMouseEvent *>(event);
        //e._pos[EViewportPos] = msEvent->pos();
        e._pos[EViewportPos] = /*msEvent->pos()*/scene->drawView()->viewport()->mapFromGlobal(msEvent->globalPos());
        e._pos[EGlobelPos]   = msEvent->globalPos();
        e._msBtns = msEvent->button() | msEvent->buttons();
        e._kbMods = msEvent->modifiers();
        e._orgEvent = event;
        e._scene    = scene;
        eList.insert(0, e);
        break;
    }
    case QEvent::GraphicsSceneMouseMove:
    case QEvent::GraphicsSceneMousePress:
    case QEvent::GraphicsSceneMouseRelease:
    case QEvent::GraphicsSceneMouseDoubleClick: {
        QGraphicsSceneMouseEvent *msEvent = dynamic_cast<QGraphicsSceneMouseEvent *>(event);
        e._pos[EViewportPos] = /*msEvent->pos()*/scene->drawView()->viewport()->mapFromGlobal(msEvent->screenPos());
        e._pos[EScenePos]    = msEvent->scenePos();
        e._pos[EGlobelPos]   = msEvent->screenPos();
        e._msBtns = msEvent->button() | msEvent->buttons();
        e._kbMods = msEvent->modifiers();
        e._orgEvent = event;
        e._scene    = scene;
        eList.insert(0, e);
        break;
    }
    case QEvent::TouchBegin:
    case QEvent::TouchUpdate:
    case QEvent::TouchEnd: {
        QTouchEvent *touchEvent = dynamic_cast<QTouchEvent *>(event);
        const QList<QTouchEvent::TouchPoint> lists = touchEvent->touchPoints();
        for (auto tPos : lists) {
            e = fromTouchPoint(tPos, scene);
            e._orgEvent = event;
            eList.insert(e.uuid(), e);
        }
        break;
    }
    default:
        break;
    }
    return eList;
}

IDrawTool::CDrawToolEvent IDrawTool::CDrawToolEvent::fromTouchPoint(const QTouchEvent::TouchPoint &tPos,
                                                                    CDrawScene *scene, QEvent *eOrg)
{
    CDrawToolEvent e;
    e._pos[EViewportPos] = tPos.pos();
    e._pos[EScenePos]    = tPos.scenePos();
    e._pos[EGlobelPos]   = tPos.screenPos();
    e._uuid              = tPos.id();
    e._scene             = scene;
    e._kbMods            = dApp->keyboardModifiers();
    e._orgEvent = eOrg;
    //qDebug() << "e._pos[EViewportPos] = " << e._pos[EViewportPos] << "e._pos[EScenePos] = " << e._pos[EScenePos] << "e._pos[EGlobelPos] = " << e._pos[EGlobelPos];
    return e;
}

QPointF IDrawTool::CDrawToolEvent::pos(IDrawTool::CDrawToolEvent::EPosType tp)
{
    if (tp >= EScenePos && tp < PosTypeCount) {
        return _pos[tp];
    }
    return QPointF(0, 0);
}

Qt::MouseButtons IDrawTool::CDrawToolEvent::mouseButtons()
{
    return _msBtns;
}

Qt::KeyboardModifiers IDrawTool::CDrawToolEvent::keyboardModifiers()
{
    return _kbMods;
}

int IDrawTool::CDrawToolEvent::uuid()
{
    return _uuid;
}

IDrawTool::CDrawToolEvent::EEventTp IDrawTool::CDrawToolEvent::eventType()
{
    if (orgQtEvent() == nullptr)
        return EEventSimulated;

    EEventTp tp = EEventSimulated;

    switch (orgQtEvent()->type()) {
    case QEvent::MouseButtonPress:
    case QEvent::MouseButtonRelease:
    case QEvent::MouseMove:
    case QEvent::MouseButtonDblClick:
    case QEvent::GraphicsSceneMouseMove:
    case QEvent::GraphicsSceneMousePress:
    case QEvent::GraphicsSceneMouseRelease:
    case QEvent::GraphicsSceneMouseDoubleClick: {
        tp = EMouseEvent;
        break;
    }
    case QEvent::TouchBegin:
    case QEvent::TouchUpdate:
    case QEvent::TouchEnd: {
        tp = ETouchEvent;
        break;
    }
    default:
        break;
    }
    return tp;
}

QEvent *IDrawTool::CDrawToolEvent::orgQtEvent()
{
    return _orgEvent;
}

CDrawScene *IDrawTool::CDrawToolEvent::scene()
{
    return _scene;
}

CGraphicsView *IDrawTool::CDrawToolEvent::view()
{
    if (_scene != nullptr) {
        return _scene->drawView();
    }
    return nullptr;
}

bool IDrawTool::CDrawToolEvent::isAccepted()
{
    return _accept;
}

void IDrawTool::CDrawToolEvent::setAccepted(bool b)
{
    _accept = b;
}

bool IDrawTool::ITERecordInfo::isVaild()
{
    return _isvaild;
}

bool IDrawTool::ITERecordInfo::hasMoved()
{
    //return (_prePos != _startPos);

    //return (_prePos - _startPos).manhattanLength() > dApp->startDragDistance();

    return _moved;
}

QTime *IDrawTool::ITERecordInfo::getTimeHandle()
{
    return &_elapsedToUpdateTimeHandle;
}
