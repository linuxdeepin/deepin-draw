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
    CDrawToolEvent::CDrawToolEvents e = CDrawToolEvent::fromQEvent(event, scene);

    toolDoStart(&e.first());

    if (!e.first().isAccepted()) {
        scene->mouseEvent(event);
    }
}

void IDrawTool::mouseMoveEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene)
{
    CDrawToolEvent::CDrawToolEvents e = CDrawToolEvent::fromQEvent(event, scene);

    toolDoUpdate(&e.first());

    //已经处理完成，不用传递到框架,否则传递给框架继续处理
    if (!e.first().isAccepted()) {
        scene->mouseEvent(event);
    }
}

void IDrawTool::mouseReleaseEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene)
{
    CDrawToolEvent::CDrawToolEvents e = CDrawToolEvent::fromQEvent(event, scene);

    toolDoFinish(&e.first());

    //已经处理完成，不用传递到框架,否则传递给框架继续处理
    if (!e.first().isAccepted()) {
        scene->mouseEvent(event);
    }
}
void IDrawTool::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene)
{
    CDrawToolEvent::CDrawToolEvents e = CDrawToolEvent::fromQEvent(event, scene);

    dueTouchDoubleClickedStart(&e.first());

    if (!e.first().isAccepted()) {
        scene->mouseEvent(event);
    }
}

bool IDrawTool::isUpdating()
{
    return m_bMousePress;
}

void IDrawTool::interruptUpdating()
{
    clearITE();
    m_bMousePress = false;
}

int IDrawTool::isUpdatingType()
{
    if (!_allITERecordInfo.isEmpty()) {
        return _allITERecordInfo.last()._opeTpUpdate;
    }
    return -1;
}

void IDrawTool::toolDoStart(IDrawTool::CDrawToolEvent *event)
{
    if (event->mouseButtons() == Qt::LeftButton || event->eventType() == CDrawToolEvent::ETouchEvent) {
        m_bMousePress = true;

        if (dueTouchDoubleClickedStart(event)) {
            return;
        }

        ITERecordInfo info;

        info._prePos = event->pos();
        info._startPos = event->pos();
        info.businessItem = creatItem(event);
        info.startPosItems = event->scene()->items(event->pos());
        info.startPosTopBzItem = event->scene()->topBzItem(event->pos(), true, 10);
        info._isvaild = true;
        info._curEvent = *event;
        info._scene = event->scene();
        info.getTimeHandle()->restart();

        _allITERecordInfo.insert(event->uuid(), info);

        if (info.businessItem != nullptr) {
            //工具开始创建图元应该清理当前选中情况
            event->scene()->clearSelection();

            toolCreatItemStart(event, &info);

            QList<QVariant> vars;
            vars << reinterpret_cast<long long>(event->scene());
            vars << reinterpret_cast<long long>(info.businessItem);

            CUndoRedoCommand::recordUndoCommand(CUndoRedoCommand::ESceneChangedCmd,
                                                CSceneUndoRedoCommand::EItemAdded, vars, true, true);

        } else {
            toolStart(event, &info);
        }
    }
}

void IDrawTool::toolDoUpdate(IDrawTool::CDrawToolEvent *event)
{
    if (m_bMousePress) {
        auto it = _allITERecordInfo.find(event->uuid());
        if (it != _allITERecordInfo.end()) {
            ITERecordInfo &rInfo = it.value();
            rInfo._preEvent = rInfo._curEvent;
            rInfo._curEvent = *event;

            if (rInfo._firstCallToolUpdate) {
                //判定移动的幅度很小
                QRectF rectf(event->view()->mapFromScene(rInfo._startPos) - QPointF(10, 10), QSizeF(20, 20));
                if (!rectf.contains(event->pos(CDrawToolEvent::EViewportPos))) {
                    QTime *elTi = rInfo.getTimeHandle();
                    rInfo._elapsedToUpdate = (elTi == nullptr ? -1 : elTi->elapsed());
                    rInfo._opeTpUpdate = decideUpdate(event, &rInfo);
                    //调用图元的operatingBegin函数
                    if (rInfo._opeTpUpdate != -1) {
                        for (auto it : rInfo.etcItems) {
                            if (event->scene()->isBussizeItem(it) || it->type() == MgrType) {
                                CGraphicsItem *pBzItem = dynamic_cast<CGraphicsItem *>(it);
                                pBzItem->operatingBegin(rInfo._opeTpUpdate);
                            }
                        }
                    }
                    rInfo._firstCallToolUpdate = false;
                }
            }

            if (rInfo._opeTpUpdate != -1) {
                if (rInfo.businessItem != nullptr) {
                    toolCreatItemUpdate(event, &rInfo);
                } else {
                    toolUpdate(event, &rInfo);
                }
            }

            rInfo._prePos = event->pos();

        } else {
            //toolDoStart(event);
        }
    } else {
        mouseHoverEvent(event);
    }
}

void IDrawTool::toolDoFinish(IDrawTool::CDrawToolEvent *event)
{
    if (m_bMousePress) {
        auto it = _allITERecordInfo.find(event->uuid());
        if (it != _allITERecordInfo.end()) {
            ITERecordInfo &rInfo = it.value();
            rInfo._prePos = event->pos();
            rInfo._preEvent = rInfo._curEvent;
            rInfo._curEvent = *event;

            //调用图元的operatingBegin函数
            if (rInfo._opeTpUpdate != -1) {
                for (auto it : rInfo.etcItems) {
                    if (event->scene()->isBussizeItem(it) || it->type() == MgrType) {
                        CGraphicsItem *pBzItem = dynamic_cast<CGraphicsItem *>(it);
                        pBzItem->operatingEnd(rInfo._opeTpUpdate);
                    }
                }
            }

            if (rInfo.businessItem != nullptr) {


                event->scene()->selectItem(rInfo.businessItem);

                QList<QVariant> vars;
                vars << reinterpret_cast<long long>(event->scene());
                vars << reinterpret_cast<long long>(rInfo.businessItem);

                CUndoRedoCommand::recordRedoCommand(CUndoRedoCommand::ESceneChangedCmd,
                                                    CSceneUndoRedoCommand::EItemAdded, vars);

                CUndoRedoCommand::finishRecord();

                setViewToSelectionTool(event->scene()->drawView());

                toolCreatItemFinish(event, &rInfo);

            } else {
                toolFinish(event, &rInfo);
            }

            _allITERecordInfo.erase(it);
        }
        m_bMousePress = false;

        event->scene()->refreshLook(event->pos());
    }
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
            QRectF rectf(event->view()->mapFromScene(prePos) - QPointF(10, 10), QSizeF(20, 20));
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

    //默认传递事件给框架
    event->setAccepted(false);
}

void IDrawTool::toolUpdate(IDrawTool::CDrawToolEvent *event, ITERecordInfo *pInfo)
{
    Q_UNUSED(pInfo)

    //默认传递事件给框架
    event->setAccepted(false);
}

void IDrawTool::toolFinish(IDrawTool::CDrawToolEvent *event, ITERecordInfo *pInfo)
{
    Q_UNUSED(pInfo)

    //默认传递事件给框架
    event->setAccepted(false);

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

IDrawTool::ITERecordInfo *IDrawTool::getEventIteInfo(int uuid)
{
    auto itf = _allITERecordInfo.find(uuid);
    if (itf != _allITERecordInfo.end()) {
        return &itf.value();
    }
    return nullptr;
}

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
        e._pos[EViewportPos] = msEvent->pos();
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
        e._pos[EViewportPos] = msEvent->pos();
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

QTime *IDrawTool::ITERecordInfo::getTimeHandle()
{
    return &_elapsedToUpdateTimeHandle;
}
