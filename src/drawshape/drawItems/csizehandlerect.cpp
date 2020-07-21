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
#include "csizehandlerect.h"
#include "cdrawparamsigleton.h"
#include "frame/cviewmanagement.h"
#include "frame/cgraphicsview.h"

#include "cdrawtoolmanagersigleton.h"
#include "cdrawscene.h"
#include "cselecttool.h"
#include "cgraphicsitem.h"

#include <QGraphicsScene>
#include <QGraphicsSceneContextMenuEvent>
#include <QMenu>
#include <QPainter>
#include <QDebug>
#include <QString>

CSizeHandleRect::CSizeHandleRect(QGraphicsItem *parent, EDirection d)
    : QGraphicsSvgItem(parent)
    , m_dir(d)
    , m_state(SelectionHandleOff)
    , m_bVisible(true)
    , m_lightRenderer(QString(":/theme/light/images/size/node.svg"))
    , m_darkRenderer(QString(":/theme/dark/images/size/node-dark.svg"))
    , m_isRotation(false)
{
    setParentItem(parent);
    setCacheMode(NoCache);
    setSharedRenderer(&m_lightRenderer);
    //hide();
    setFlag(ItemIsSelectable, false);
    setFlag(ItemIsMovable, false);
}

CSizeHandleRect::CSizeHandleRect(QGraphicsItem *parent, CSizeHandleRect::EDirection d, const QString &filename)
    : QGraphicsSvgItem(filename, parent)
    , m_dir(d)
    , m_state(SelectionHandleOff)
    , m_bVisible(true)
    , m_isRotation(true)
{
    setParentItem(parent);
    setCacheMode(NoCache);
    //hide();

    setFlag(ItemIsSelectable, false);
    setFlag(ItemIsMovable, false);
}

CGraphicsView *CSizeHandleRect::curView() const
{
    CGraphicsView *parentView = nullptr;
    if (scene() != nullptr) {
        if (!scene()->views().isEmpty()) {
            parentView = dynamic_cast<CGraphicsView *>(scene()->views().first());
        }
    }
    return parentView;
}

void CSizeHandleRect::updateCursor()
{
    switch (m_dir) {
    case Right:
        setCursor(Qt::SizeHorCursor);
        return;
    case RightTop:
        setCursor(Qt::SizeBDiagCursor);
        return;
    case RightBottom:
        setCursor(Qt::SizeFDiagCursor);
        return;
    case LeftBottom:
        setCursor(Qt::SizeBDiagCursor);
        return;
    case Bottom:
        setCursor(Qt::SizeVerCursor);
        return;
    case LeftTop:
        setCursor(Qt::SizeFDiagCursor);
        return;
    case Left:
        setCursor(Qt::SizeHorCursor);
        return;
    case Top:
        setCursor(Qt::SizeVerCursor);
        return;
    default:
        break;
    }
    setCursor(Qt::ArrowCursor);
}

void CSizeHandleRect::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)

    //如果仅存在功能那么什么都不用绘制了
    if (m_onlyLogicAblity)
        return;

    if (isFatherDragging())
        return;

    if (!m_isRotation) {
        if (/* CManageViewSigleton::GetInstance()->getThemeType() == 1 && */renderer() != &m_lightRenderer) {
            setSharedRenderer(&m_lightRenderer);
        } /*else if (CManageViewSigleton::GetInstance()->getThemeType() == 2 && renderer() != &m_darkRenderer) {
            setSharedRenderer(&m_darkRenderer);
        }*/
    }

    painter->setClipping(false);
    QRectF rect = this->boundingRect();

    this->renderer()->render(painter, rect);
    painter->setClipping(true);

}

bool CSizeHandleRect::isFatherDragging()
{
    CGraphicsItem *pParentItem = dynamic_cast<CGraphicsItem *>(parentItem());
    if (pParentItem != nullptr) {
        CDrawScene *pDrawScene = qobject_cast<CDrawScene *>(scene());
        if (pDrawScene != nullptr) {
            EDrawToolMode model = pDrawScene->getDrawParam()->getCurrentDrawToolMode();
            if (model == selection) {
                CSelectTool *pTool = dynamic_cast<CSelectTool *>(CDrawToolManagerSigleton::GetInstance()->getDrawTool(model));
                return (pTool != nullptr && pTool->isDragging());
            }
        }
    }
    return false;
}

void CSizeHandleRect::initCursor()
{
    QPixmap m_RotateCursor(QPixmap(":/theme/light/images/mouse_style/rotate_mouse.svg").scaled(24, 24, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));

    QPixmap m_LeftTopCursor(QPixmap(":/theme/light/images/mouse_style/icon_drag_leftup.svg").scaled(24, 24, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));

    QPixmap m_RightTopCursor(QPixmap(":/theme/light/images/mouse_style/icon_drag_rightup.svg").scaled(24, 24, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));

    QPixmap m_LeftRightCursor(QPixmap(":/theme/light/images/mouse_style/icon_drag_left.svg").scaled(24, 24, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    QPixmap m_UpDownCursor(QPixmap(":/theme/light/images/mouse_style/icon_drag_up.svg").scaled(24, 24, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
}

void CSizeHandleRect::setState(ESelectionHandleState st)
{
    if (st != m_state) {
        switch (st) {
        case SelectionHandleOff:
            hide();
            break;
        case SelectionHandleInactive:
        case SelectionHandleActive:
            if (m_bVisible) {
                show();
            }
            break;
        }
        m_state = st;
    }
}

bool CSizeHandleRect::hitTest(const QPointF &point)
{
    bool bRet = false;
    if (m_bVisible) {
        QPointF pt = mapFromScene(point);
        bRet = this->boundingRect().contains(pt);
    }
    return bRet;
}

void CSizeHandleRect::move(qreal x, qreal y)
{
    setPos(x, y);
}

QRectF CSizeHandleRect::boundingRect() const
{
    if (curView() == nullptr)
        return QRectF();

    qreal scale = curView()->getDrawParam()->getScale();
    QRectF rect = QGraphicsSvgItem::boundingRect();
    rect.setWidth(rect.width() / scale);
    rect.setHeight(rect.height() / scale);
    return rect;
}

void CSizeHandleRect::setVisible(bool flag)
{
    m_bVisible = flag;
    if (parentItem() != nullptr && parentItem()->isSelected()) {
        m_bVisible ? show() : hide();
    }
}

bool CSizeHandleRect::getVisible() const
{
    return m_bVisible;
}

void CSizeHandleRect::setJustExitLogicAbility(bool b)
{
    m_onlyLogicAblity = b;
    update();
}

QCursor CSizeHandleRect::getCursor()
{
    static QPixmap m_RotateCursor(QPixmap(":/theme/light/images/mouse_style/rotate_mouse.svg").scaled(24, 24, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));

    static QPixmap m_LeftTopCursor(QPixmap(":/theme/light/images/mouse_style/icon_drag_leftup.svg").scaled(24, 24, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));

    static QPixmap m_RightTopCursor(QPixmap(":/theme/light/images/mouse_style/icon_drag_rightup.svg").scaled(24, 24, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));

    static QPixmap m_LeftRightCursor(QPixmap(":/theme/light/images/mouse_style/icon_drag_left.svg").scaled(24, 24, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    static QPixmap m_UpDownCursor(QPixmap(":/theme/light/images/mouse_style/icon_drag_up.svg").scaled(24, 24, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));

    QCursor cursorResult(Qt::ArrowCursor);
    QMatrix matrix;
    qreal rotaAngle = parentItem() == nullptr ? 0 : parentItem()->rotation();
    matrix.rotate(rotaAngle);

    switch (m_dir) {
    case Right:
    case Left:
        cursorResult = QCursor(m_LeftRightCursor.transformed(matrix, Qt::SmoothTransformation));
        break;
    case Top:
    case Bottom:
        cursorResult = QCursor(m_UpDownCursor.transformed(matrix, Qt::SmoothTransformation));
        break;
    case RightTop:
    case LeftBottom:
        cursorResult = QCursor(m_RightTopCursor.transformed(matrix, Qt::SmoothTransformation));
        break;
    case LeftTop:
    case RightBottom:
        cursorResult = QCursor(m_LeftTopCursor.transformed(matrix, Qt::SmoothTransformation));
        break;

    case Rotation:
        cursorResult = QCursor(m_RotateCursor.transformed(matrix, Qt::SmoothTransformation));
        break;

    default:
        break;
    }

    return cursorResult;
}
