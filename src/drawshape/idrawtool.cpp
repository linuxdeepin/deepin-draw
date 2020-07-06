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
void IDrawTool::mousePressEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene)
{
    //鼠标左键按下走和触控统一的逻辑
    if (event->button() == Qt::LeftButton) {
        m_bMousePress = true;

        CDrawToolEvent::CDrawToolEvents e = CDrawToolEvent::fromQEvent(event, scene);
        toolStart(&e.first());

        //已经处理完成，不用传递到框架,否则传递给框架继续处理
        if (!e.first().isAccepted()) {
            scene->mouseEvent(event);
        }

        return;
    }

    //默认是要传递给框架的
    scene->mouseEvent(event);
}

void IDrawTool::mouseMoveEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene)
{
    if (event->buttons() == Qt::LeftButton) {
        CDrawToolEvent::CDrawToolEvents e = CDrawToolEvent::fromQEvent(event, scene);
        toolUpdate(&e.first());

        //已经处理完成，不用传递到框架,否则传递给框架继续处理
        if (!e.first().isAccepted()) {
            scene->mouseEvent(event);
        }

        return;
    }

    //默认是要传递给框架的
    scene->mouseEvent(event);
}

void IDrawTool::mouseReleaseEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene)
{
    if (event->button() == Qt::LeftButton) {
        m_bMousePress = false;

        CDrawToolEvent::CDrawToolEvents e = CDrawToolEvent::fromQEvent(event, scene);

        toolFinish(&e.first());

        //已经处理完成，不用传递到框架,否则传递给框架继续处理
        if (!e.first().isAccepted()) {
            scene->mouseEvent(event);
        }

        return;
    }

    //默认是要传递给框架的
    scene->mouseEvent(event);
}
void IDrawTool::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene)
{
    //鼠标左键按下走和触控统一的逻辑
    if (event->button() == Qt::LeftButton) {
        m_bMousePress = true;

        CDrawToolEvent::CDrawToolEvents e = CDrawToolEvent::fromQEvent(event, scene);
        toolStart(&e.first());

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

void IDrawTool::toolStart(IDrawTool::CDrawToolEvent *event)
{
    ITERecordInfo info;

    info._prePos = event->pos();
    info._startPos = event->pos();
    info.businessItem = creatItem(event);
    info.startPosItems = event->scene()->items(event->pos());
    info._isvaild = true;

    _allITERecordInfo.insert(event->uuid(), info);

    //工具开始创建图元应该清理当前选中情况
    event->scene()->clearSelection();

    //默认不传递事件给框架
    event->setAccepted(true);
}

void IDrawTool::toolUpdate(IDrawTool::CDrawToolEvent *event)
{
    auto it = _allITERecordInfo.find(event->uuid());
    if (it != _allITERecordInfo.end()) {
        it.value()._prePos = event->pos();
    } else {
        toolStart(event);
    }

    //默认不传递事件给框架
    event->setAccepted(true);
}

void IDrawTool::toolFinish(IDrawTool::CDrawToolEvent *event)
{
    _allITERecordInfo.remove(event->uuid());

    //默认不传递事件给框架
    event->setAccepted(true);
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
