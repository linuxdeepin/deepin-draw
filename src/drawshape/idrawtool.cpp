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
    //鼠标左键按下走和触控统一的逻辑
    if (event->button() == Qt::LeftButton) {
        m_bMousePress = true;

        CDrawToolEvent::CDrawToolEvents e = CDrawToolEvent::fromQEvent(event, scene);
        toolDoStart(&e.first());

        //已经处理完成，不用传递到框架,否则传递给框架继续处理
        if (!e.first().isAccepted()) {
            scene->mouseEvent(event);
        }

        return;
    }

    //默认是要传递给框架的
    scene->mouseEvent(event);
}

bool IDrawTool::isCreating()
{
    return m_bMousePress;
}

void IDrawTool::stopCreating()
{
    m_bMousePress = false;
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

void IDrawTool::toolDoStart(IDrawTool::CDrawToolEvent *event)
{
    if (event->mouseButtons() == Qt::LeftButton || event->eventType() == CDrawToolEvent::ETouchEvent) {
        m_bMousePress = true;

        ITERecordInfo info;

        info._prePos = event->pos();
        info._startPos = event->pos();
        info.businessItem = creatItem(event);
        info.startPosItems = event->scene()->items(event->pos());
        info.startPosTopBzItem = event->scene()->topBzItem(event->pos());
        info._isvaild = true;
        info._curEvent = *event;
        info._scene = event->scene();
        info.getTimeHandle()->start();

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
            toolDoStart(event);
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
            it.value()._prePos = event->pos();
            it.value()._preEvent = it.value()._curEvent;
            it.value()._curEvent = *event;

            if (it.value().businessItem != nullptr) {
                toolCreatItemFinish(event, &it.value());

                event->scene()->selectItem(it.value().businessItem);

                QList<QVariant> vars;
                vars << reinterpret_cast<long long>(event->scene());
                vars << reinterpret_cast<long long>(it.value().businessItem);

                CUndoRedoCommand::recordRedoCommand(CUndoRedoCommand::ESceneChangedCmd,
                                                    CSceneUndoRedoCommand::EItemAdded, vars);

                CUndoRedoCommand::finishRecord();

                setViewToSelectionTool(event->scene()->drawView());

            } else {
                toolFinish(event, &it.value());
            }

            _allITERecordInfo.erase(it);
        }
        m_bMousePress = false;

        event->scene()->refreshLook(event->pos());
    }
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
