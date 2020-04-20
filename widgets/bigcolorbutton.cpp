/*
 * Copyright (C) 2019 ~ %YEAR% Deepin Technology Co., Ltd.
 *
 * Author:     Renran
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
#include "bigcolorbutton.h"

#include "utils/baseutils.h"
#include "drawshape/cdrawparamsigleton.h"
#include "frame/cviewmanagement.h"
#include "frame/cgraphicsview.h"

#include <QDebug>
#include <QTextItem>

const qreal COLOR_RADIUS = 4;
const int BTN_RADIUS_BEGING = 8;
const QPoint CENTER_POINT = QPoint(12, 12);

BigColorButton::BigColorButton(DWidget *parent)
    : DPushButton(parent)
    , m_isHover(false)
    , m_isChecked(false)
    , m_isMultColorSame(true)
{
    setFixedSize(63, 32);
    setCheckable(false);

    m_color = CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getFillColor();
}

void BigColorButton::updateConfigColor()
{
    m_isMultColorSame = true;
    QColor configColor = CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getFillColor();

    if (m_color == configColor) {
        return;
    }
    m_color = configColor;
    update();
}

BigColorButton::~BigColorButton()
{
}

void BigColorButton::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    if (m_isMultColorSame) {
        painter.setRenderHints(QPainter::Antialiasing
                               | QPainter::SmoothPixmapTransform);
        painter.save();
        painter.setPen(Qt::transparent);
        QPainterPath clipPath;
        clipPath.addRoundedRect(QRect(1, 7, 20, 20),  8, 8);
        painter.setClipPath(clipPath);
        QColor drawColor = m_color;

        QPen borderPen;
        borderPen.setWidth(1);
        if (CManageViewSigleton::GetInstance()->getThemeType() == 1) {
            borderPen.setColor(QColor(0, 0, 0, 25));
        } else {
            borderPen.setColor(QColor(255, 255, 255, 25));
        }
        if (m_color == QColor(Qt::transparent) || m_color.alpha() == 0) {
            if (CManageViewSigleton::GetInstance()->getThemeType() == 1) {
                painter.setBrush(Qt::NoBrush);
            } else {
                painter.setBrush(QColor(8, 15, 21, 178));
            }
            if (CManageViewSigleton::GetInstance()->getThemeType() == 1) {
                borderPen.setColor(QColor(0, 0, 0, 25));
            } else {
                borderPen.setColor(QColor(77, 82, 93, 204));
            }
        } else {
            painter.setBrush(drawColor);
        }
        painter.setPen(borderPen);
        painter.drawRoundedRect(QRect(1, 7, 20, 20),  8, 8);

        if (m_color == QColor(Qt::transparent) || m_color.alpha() == 0) {
            QPen linePen;
            linePen.setWidth(2);
            linePen.setColor(QColor(255, 67, 67, 153));
            painter.setPen(linePen);
            painter.drawLine(4, 23, 18, 11);
        }
        painter.restore();
        QPen textPen;
        if (CManageViewSigleton::GetInstance()->getThemeType() == 1) {
            textPen.setColor(QColor("#414D68"));
        } else {
            textPen.setColor(QColor("#C0C6D4"));
        }

        painter.setPen(textPen);
        QFont ft;
        ft.setPixelSize(14);
        painter.setFont(ft);

        painter.drawText(27, 6, 28, 22, 0, tr("Fill"));
    } else {
        painter.setRenderHints(QPainter::Antialiasing
                               | QPainter::SmoothPixmapTransform );
        painter.save();
        QPainterPath clipPath;
        clipPath.addRoundedRect(QRect(1, 7, 20, 20),  8, 8);
        painter.setClipPath(clipPath);
        if (CManageViewSigleton::GetInstance()->getThemeType() == 1) {
            painter.setBrush(QColor(0, 0, 0, 12));
        } else {
            painter.setBrush(QColor(8, 15, 21, 179));
        }
        painter.drawRoundedRect(QRect(1, 7, 20, 20),  8, 8);

        QPen borderPen;
        borderPen.setWidth(1);
        if (CManageViewSigleton::GetInstance()->getThemeType() == 1) {
            borderPen.setColor(QColor(0, 0, 0, 12));
        } else {
            borderPen.setColor(QColor(77, 82, 93, 204));
        }
        painter.setPen(borderPen);
        painter.drawRoundedRect(QRect(1, 7, 20, 20),  8, 8);
        painter.restore();
        QPen textPen;
        if (CManageViewSigleton::GetInstance()->getThemeType() == 1) {
            textPen.setColor(QColor("#414D68"));
        } else {
            textPen.setColor(QColor("#C0C6D4"));
        }
        painter.setPen(textPen);
        QFont ft;
        ft.setPixelSize(14);
        painter.setFont(ft);
        painter.drawText(5, 1, 32, 16, 0, tr("..."));

        painter.setPen(textPen);
        ft.setPixelSize(14);
        painter.setFont(ft);
        painter.drawText(27, 6, 28, 22, 0, tr("Fill"));
    }

}

void BigColorButton::setColor(QColor color)
{
    m_isMultColorSame = true;
    m_color = color;
    update();
}

void BigColorButton::setColorIndex(int index)
{
    m_color = colorIndexOf(index);
    update();
}

void BigColorButton::enterEvent(QEvent *)
{
    if (!m_isHover) {
        m_isHover = true;
        update();
    }
}

void BigColorButton::leaveEvent(QEvent *)
{
    if (m_isHover) {
        m_isHover = false;
        update();
    }
}

void BigColorButton::mousePressEvent(QMouseEvent * )
{
    m_isChecked = !m_isChecked;
    update();

    btnCheckStateChanged(m_isChecked);
}

void BigColorButton::resetChecked()
{
    m_isChecked = false;
    update();
}

void BigColorButton::setIsMultColorSame(bool isContainColor)
{
    m_isMultColorSame = isContainColor;
}
