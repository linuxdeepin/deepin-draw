/*
 * Copyright (C) 2019 ~ %YEAR% Deepin Technology Co., Ltd.
 *
 * Author:     WangXing
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
#include <QDebug>
#include <QKeyEvent>


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
    , m_mode(mode)
    , m_RotateCursor(QPixmap(":/theme/light/images/mouse_style/rotate_mouse.svg"))
{

}

EDrawToolMode IDrawTool::getDrawToolMode() const
{
    return m_mode;
}

QCursor IDrawTool::getCursor(CSizeHandleRect::EDirection dir, bool bMouseLeftPress)
{
    Qt::CursorShape result;
    QCursor resultCursor;
    switch (dir) {
    case CSizeHandleRect::Right:
        if (bMouseLeftPress) {
            result =  Qt::ClosedHandCursor;
        } else {
            result =  Qt::SizeHorCursor;
        }
        resultCursor = QCursor(result);
        break;
    case CSizeHandleRect::RightTop:
        if (bMouseLeftPress) {
            result =  Qt::ClosedHandCursor;
        } else {
            result =  Qt::SizeBDiagCursor;
        }
        resultCursor = QCursor(result);
        break;
    case CSizeHandleRect::RightBottom:
//        result =  Qt::SizeFDiagCursor;
        if (bMouseLeftPress) {
            result =  Qt::ClosedHandCursor;
        } else {
            result =  Qt::SizeFDiagCursor;
        }
        resultCursor = QCursor(result);
        break;
    case CSizeHandleRect::LeftBottom:
        if (bMouseLeftPress) {
            result =  Qt::ClosedHandCursor;
        } else {
            result =  Qt::SizeBDiagCursor;
        }
        resultCursor = QCursor(result);
        break;
    case CSizeHandleRect::Bottom:
        if (bMouseLeftPress) {
            result =  Qt::ClosedHandCursor;
        } else {
            result =  Qt::SizeVerCursor;
        }
        resultCursor = QCursor(result);
        break;
    case CSizeHandleRect::LeftTop:
        if (bMouseLeftPress) {
            result =  Qt::ClosedHandCursor;
        } else {
            result =  Qt::SizeFDiagCursor;
        }
        resultCursor = QCursor(result);
        break;
    case CSizeHandleRect::Left:
        if (bMouseLeftPress) {
            result =  Qt::ClosedHandCursor;
        } else {
            result =  Qt::SizeHorCursor;
        }
        resultCursor = QCursor(result);
        break;
    case CSizeHandleRect::Top:
        if (bMouseLeftPress) {
            result =  Qt::ClosedHandCursor;
        } else {
            result =  Qt::SizeVerCursor;
        }
        resultCursor = QCursor(result);
        break;

    case CSizeHandleRect::Rotation:
        resultCursor = m_RotateCursor;
        break;
    case CSizeHandleRect::InRect:
        if (bMouseLeftPress) {
            result =  Qt::ClosedHandCursor;
        } else {
            result =  Qt::OpenHandCursor;
        }
        resultCursor = QCursor(result);
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
