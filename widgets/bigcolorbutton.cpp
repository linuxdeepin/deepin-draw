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

#include <QDebug>
#include <QTextItem>

const qreal COLOR_RADIUS = 4;
const int BTN_RADIUS_BEGING = 8;
const QPoint CENTER_POINT = QPoint(12, 12);

BigColorButton::BigColorButton(DWidget *parent)
    : DPushButton(parent)
    , m_isHover(false)
    , m_isChecked(false)
{
    //setFixedSize(24, 24);
    setFixedSize(56, 36);
    setCheckable(false);

    m_color = CDrawParamSigleton::GetInstance()->getFillColor();
}

void BigColorButton::updateConfigColor()
{
    QColor configColor = CDrawParamSigleton::GetInstance()->getFillColor();

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
    painter.setRenderHints(QPainter::Antialiasing
                           | QPainter::SmoothPixmapTransform);
    painter.setPen(Qt::transparent);

    //painter.setBrush(QBrush(QColor(0, 0, 0, 13)));
    QColor drawColor = m_color;

//    if (m_isHover || m_isChecked) {
//        painter.setBrush(QBrush(QColor(0, 0, 0, 25)));
//        painter.drawRoundedRect(rect(), 6, 6);
//    } else if (m_isChecked) {
//        drawColor = QColor(m_color.red(), m_color.green(), m_color.blue(), 25);
//    }

    painter.setBrush(drawColor);
    //painter.drawEllipse(CENTER_POINT,  BTN_RADIUS, BTN_RADIUS);
    painter.drawRoundedRect(QRect(4, 10, 16, 16),  6, 6);

    QPen borderPen;
    borderPen.setWidth(1);
    //borderPen.setColor(QColor(0, 0, 0, 15));
    //borderPen.setColor(Qt::gray);
    if (m_color == Qt::transparent || m_color == QColor("#ffffff")) {
        borderPen.setColor(Qt::gray);
    } else {
        borderPen.setColor(Qt::transparent);
    }
    painter.setPen(borderPen);
//    if (m_isChecked) {
//        painter.setBrush(QColor(0, 0, 0, 55));
//    } else {
//        painter.setBrush(Qt::transparent);
//    }
    //painter.drawEllipse(CENTER_POINT, BTN_RADIUS + 1, BTN_RADIUS + 1);
    painter.drawRoundedRect(QRect(4, 10, 16, 16),  6, 6);

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

    painter.drawText(26, 9, 32, 16, 0, tr("Fill"));

}

void BigColorButton::setColor(QColor color)
{
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
