/*
 * Copyright (C) 2019 ~ %YEAR% Deepin Technology Co., Ltd.
 *
 * Author:     RenRan
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
#include "bordercolorbutton.h"

#include <QPainter>
#include <QDebug>

#include "utils/baseutils.h"
#include "drawshape/cdrawparamsigleton.h"

const qreal COLOR_RADIUS = 4;
const int BTN_RADIUS = 8;
const QPoint CENTER_POINT = QPoint(12, 12);

BorderColorButton::BorderColorButton(DWidget *parent)
    : DPushButton(parent)
    , m_isHover(false)
    , m_isChecked(false)
{
    setFixedSize(62, 36);
    setCheckable(false);
    m_color = CDrawParamSigleton::GetInstance()->getLineColor();
}

void BorderColorButton::updateConfigColor()
{
    QColor configColor = CDrawParamSigleton::GetInstance()->getLineColor();

    if (m_color == configColor) {
        return;
    }

    m_color = configColor;

    update();
}

BorderColorButton::~BorderColorButton()
{
}

void BorderColorButton::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing
                           | QPainter::SmoothPixmapTransform);
    painter.setPen(Qt::transparent);

    QColor drawColor = m_color;

//    if (m_isChecked || m_isHover) {
//        painter.setBrush(QBrush(QColor(0, 0, 0, 25)));
//        painter.drawRoundedRect(rect(), 6, 6);
//    } else if (m_isChecked) {
//        drawColor = QColor(m_color.red(), m_color.green(), m_color.black(), 25);
//    } else {
//        painter.setBrush(Qt::transparent);
//        painter.drawRoundedRect(rect(), 6, 6);
//    }

    QPen pen;
    pen.setWidth(2);
    pen.setColor(drawColor);
    painter.setPen(pen);
    // painter.setBrush(Qt::transparent);
    //painter.drawEllipse(CENTER_POINT, BTN_RADIUS, BTN_RADIUS);
    painter.drawRoundedRect(QRect(4, 10, 16, 16), 6, 6);

    QPen borderPen;
    borderPen.setWidth(1);
    //borderPen.setColor(QColor(0, 0, 0, 15));
    if (m_color == Qt::transparent || m_color == QColor("#ffffff")) {
        borderPen.setColor(Qt::gray);
    } else {
        borderPen.setColor(Qt::transparent);
    }
    //borderPen.setColor(Qt::gray);
    painter.setPen(borderPen);
    //painter.drawEllipse(CENTER_POINT, BTN_RADIUS + 1, BTN_RADIUS + 1);
    painter.drawRoundedRect(QRect(4, 10, 16, 16), 6, 6);

    if (m_isChecked) {
        //painter.setBrush(QColor(0, 0, 0, 35));
        //painter.drawEllipse(CENTER_POINT, BTN_RADIUS - 1, BTN_RADIUS - 1);
        painter.drawRoundedRect(QRect(5, 11, 14, 14), 6, 6);
    }

    QPen textPen;
    if (CDrawParamSigleton::GetInstance()->getThemeType() == 1) {
        textPen.setColor(QColor("#414D68"));
    } else {
        textPen.setColor(QColor("#C0C6D4"));
    }

    painter.setPen(textPen);
    QFont ft;
    ft.setPixelSize(12);
    painter.setFont(ft);

    painter.drawText(26, 9, 38, 16, 1, tr("Stroke"));
}

void BorderColorButton::setColor(QColor color)
{
    m_color = color;
    update();
}

void BorderColorButton::setColorIndex(int index)
{
    m_color = colorIndexOf(index);
    update();
}

void BorderColorButton::resetChecked()
{
    m_isChecked = false;
    update();
}

void BorderColorButton::enterEvent(QEvent *)
{
    if (!m_isHover) {
        m_isHover = true;
        update();
    }
}

void BorderColorButton::leaveEvent(QEvent *)
{
    if (m_isHover) {
        m_isHover = false;
        update();
    }
}

void BorderColorButton::mousePressEvent(QMouseEvent * )
{
    m_isChecked = !m_isChecked;
    btnCheckStateChanged(m_isChecked);

    update();
}
