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
#include "application.h"
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
    , m_RotateCursor(QPixmap(":/theme/light/images/mouse_style/rotate_mouse.svg"))
    , m_LeftTopCursor(QPixmap(":/theme/light/images/mouse_style/icon_drag_leftup.svg"))
    , m_RightTopCursor(QPixmap(":/theme/light/images/mouse_style/icon_drag_rightup.svg"))
    , m_LeftRightCursor(QPixmap(":/theme/light/images/mouse_style/icon_drag_left.svg"))
    , m_UpDownCursor(QPixmap(":/theme/light/images/mouse_style/icon_drag_up.svg"))
{

}

void IDrawTool::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene)
{
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

void IDrawTool::toolStart(IDrawTool::CDrawToolEvent *event)
{
    SRecordedStartInfo info;
    info.m_sLastPress  = event->pos();
    info.m_sPointPress = event->pos();
    info.tempItem      = creatItem();
    allStartInfo.insert(event->uuid(), info);
}

void IDrawTool::toolUpdate(IDrawTool::CDrawToolEvent *event)
{
    Q_UNUSED(event)
    auto it = allStartInfo.find(event->uuid());
    if (it != allStartInfo.end()) {
        it.value().m_sLastPress = event->pos();
    } else {
        toolStart(event);
    }
}

void IDrawTool::toolFinish(IDrawTool::CDrawToolEvent *event)
{
    Q_UNUSED(event)
    allStartInfo.remove(event->uuid());
}

void IDrawTool::toolClear()
{
    allStartInfo.clear();
}

CGraphicsItem *IDrawTool::creatItem()
{
    return nullptr;
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
    //result =  Qt::ClosedHandCursor;
    default:
        result =  Qt::ArrowCursor;
        resultCursor = QCursor(result);
        break;
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

IDrawTool::CDrawToolEvent IDrawTool::CDrawToolEvent::fromTouchPoint(const QTouchEvent::TouchPoint &tPos, CDrawScene *scene)
{
    CDrawToolEvent e;
    e._pos[EViewportPos] = tPos.pos();
    e._pos[EScenePos]    = tPos.scenePos();
    e._pos[EGlobelPos]   = tPos.screenPos();
    e._uuid              = tPos.id();
    e._scene             = scene;
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

QEvent *IDrawTool::CDrawToolEvent::orgQtEvent()
{
    return _orgEvent;
}

CDrawScene *IDrawTool::CDrawToolEvent::scene()
{
    return _scene;
}
