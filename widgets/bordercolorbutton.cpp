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
#include "frame/cviewmanagement.h"
#include "frame/cgraphicsview.h"

const qreal COLOR_RADIUS = 4;
const int BTN_RADIUS = 8;
const QPoint CENTER_POINT = QPoint(12, 12);

BorderColorButton::BorderColorButton(DWidget *parent)
    : DPushButton(parent)
    , m_isHover(false)
    , m_isChecked(false)
    , m_isMultColorSame(false)
{
    setCheckable(false);
    setButtonText(tr("Stroke"));
    m_color = CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getLineColor();
}

void BorderColorButton::updateConfigColor()
{
    m_isMultColorSame = true;
    QColor configColor = CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getLineColor();

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
    if (m_isMultColorSame) {
        painter.setRenderHints(QPainter::Antialiasing
                               | QPainter::SmoothPixmapTransform);
        painter.setPen(Qt::transparent);
        QPen colorPen;
        colorPen.setWidth(3);
        if (CManageViewSigleton::GetInstance()->getThemeType() == 1) {
            colorPen.setColor(m_color);
        } else {
            if (m_color == QColor(Qt::transparent) || m_color.alpha() == 0) {
                colorPen.setColor(QColor(8, 15, 21, 178));
            } else {
                colorPen.setColor(m_color);
            }
        }
        painter.setPen(colorPen);
        painter.drawRoundedRect(QRect(5, 9, 18, 18), 7, 7);

        QPen borderPen;
        borderPen.setWidth(1);
        if (CManageViewSigleton::GetInstance()->getThemeType() == 1) {
            borderPen.setColor(QColor(0, 0, 0, 25));
        } else {
            borderPen.setColor(QColor(77, 82, 93, 204));
        }
        painter.setPen(borderPen);
        painter.drawRoundedRect(QRect(6, 10, 16, 16), 6, 6);
        painter.drawRoundedRect(QRect(4, 8, 20, 20), 8, 8);

        if (m_color == QColor(Qt::transparent) || m_color.alpha() == 0) {
            QPen linePen;
            linePen.setWidth(2);
            linePen.setColor(QColor(255, 67, 67, 153));
            painter.setPen(linePen);
            painter.drawLine(7, 25, 22, 12);
        }

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
        painter.drawText(32, 6, m_textWidth, 22, 1, m_text);
    } else {
        painter.setRenderHints(QPainter::Antialiasing
                               | QPainter::SmoothPixmapTransform);
        QPen pen;
        pen.setWidth(1);
        if (CManageViewSigleton::GetInstance()->getThemeType() == 1) {
            pen.setColor(QColor(0, 0, 0, 12));
        } else {
            pen.setColor(QColor(77, 82, 93, 204));
        }
        painter.setPen(pen);
        painter.drawRoundedRect(QRect(4, 8, 19, 19), 8, 8);

        QPen borderPen;
        borderPen.setWidth(1);
        if (CManageViewSigleton::GetInstance()->getThemeType() == 1) {
            borderPen.setColor(QColor(0, 0, 0, 12));
        } else {
            borderPen.setColor(QColor(77, 82, 93, 204));
        }
        painter.setPen(borderPen);
        painter.drawRoundedRect(QRect(6, 10, 15, 15), 6, 6);

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
        painter.drawText(32, 6, m_textWidth, 22, 1, m_text);
    }
}

void BorderColorButton::setColor(QColor color)
{
    m_isMultColorSame = true;
    m_color = color;
    update();
}

void BorderColorButton::setColorIndex(int index)
{
    m_isMultColorSame = true;
    m_color = colorIndexOf(index);
    update();
}

void BorderColorButton::resetChecked()
{
    m_isChecked = false;
    update();
}

void BorderColorButton::setIsMultColorSame(bool isMultColorSame)
{
    m_isMultColorSame = isMultColorSame;
}

void BorderColorButton::setButtonText(QString text)
{
    QFontMetrics fontMetrics(font());
    m_textWidth = fontMetrics.width(text);
    setFixedSize(35 + m_textWidth, 32);
    m_text = text;
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
