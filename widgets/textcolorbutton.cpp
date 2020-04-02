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
#include "textcolorbutton.h"
#include "utils/baseutils.h"
#include "drawshape/cdrawparamsigleton.h"
#include "frame/cviewmanagement.h"
#include "frame/cgraphicsview.h"
#include "frame/mainwindow.h"

#include <DGuiApplicationHelper>
#include <QDebug>

DWIDGET_USE_NAMESPACE

const qreal COLOR_RADIUS = 4;
const int BTN_RADIUS = 8;
const QPoint CENTER_POINT = QPoint(12, 12);

TextColorButton::TextColorButton(DWidget *parent)
    : DPushButton(parent)
    , m_isHover(false)
    , m_isChecked(false)
    , m_isMultColorSame(true)
{
    setFixedSize(55, 36);
    setCheckable(false);

    //深色主题下
    DGuiApplicationHelper::ColorType type = DGuiApplicationHelper::instance()->themeType();
    if (DGuiApplicationHelper::instance()->themeType() == DGuiApplicationHelper::DarkType) {
        QColor color("#C0C6D4");
        CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setTextColor(color);
    }

    m_color = CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getTextColor();
}

void TextColorButton::updateConfigColor()
{
    QColor configColor = CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getTextColor();
    if (m_color == configColor) {
        return;
    }
    m_color = configColor;
    update();
}

TextColorButton::~TextColorButton()
{
}

void TextColorButton::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    if (m_isMultColorSame) {
        painter.setRenderHints(QPainter::Antialiasing
                               | QPainter::SmoothPixmapTransform);
        painter.setPen(Qt::transparent);

        painter.setBrush(QBrush(QColor(0, 0, 0, 12)));
        QColor drawColor = m_color;

        //    if (m_isHover || m_isChecked) {
        //        painter.setBrush(QBrush(QColor(0, 0, 0, 25)));
        //        painter.drawRoundedRect(rect(), 4, 4);
        //    } else if (m_isChecked) {
        //        drawColor = QColor(m_color.red(), m_color.green(), m_color.blue(), 25);
        //    }

        painter.setBrush(drawColor);
        //painter.drawEllipse(CENTER_POINT,  BTN_RADIUS, BTN_RADIUS);
        painter.drawRoundedRect(QRect(4, 8, 20, 20),  8, 8);

        QPen borderPen;
        borderPen.setWidth(1);
        //    borderPen.setColor(QColor(0, 0, 0, 15));
        //    painter.setPen(borderPen);
        //    if (m_isChecked) {
        //        painter.setBrush(QColor(0, 0, 0, 55));
        //    } else {
        //        painter.setBrush(Qt::transparent);
        //    }

        if (m_color == QColor(Qt::transparent) || m_color.alpha() == 0) {
            QPen linePen;
            linePen.setWidth(2);
            linePen.setColor(QColor("#ff804d"));
            painter.setPen(linePen);
            painter.drawLine(7, 22, 20, 12);
        }

        if (m_color == Qt::transparent || m_color == QColor("#ffffff")) {
            borderPen.setColor(QColor(0, 0, 0, 25));
        } else {
            borderPen.setColor(Qt::transparent);
        }
        if (m_color.alpha() == 0) {
            borderPen.setColor(QColor(0, 0, 0, 25));
        }
        painter.setPen(borderPen);
        //painter.drawEllipse(CENTER_POINT, BTN_RADIUS + 1, BTN_RADIUS + 1);
        painter.drawRoundedRect(QRect(4, 8, 19, 19),  8, 8);

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

        painter.drawText(26, 6, 38, 22, 0, tr("Color"));
    } else {
        painter.setRenderHints(QPainter::Antialiasing
                               | QPainter::SmoothPixmapTransform);
        QPen borderPen;
        borderPen.setWidth(1);
        borderPen.setColor(QColor(0, 0, 0, 25));
        painter.setPen(borderPen);

        painter.setBrush(QColor(0, 0, 0, 12));
        painter.drawRoundedRect(QRect(4, 8, 19, 19),  8, 8);

        QPen textPen;
        textPen.setColor(QColor(0, 0, 0, 25));
        painter.setPen(textPen);
        QFont ft;
        ft.setPixelSize(14);
        painter.setFont(ft);
        painter.drawText(8, 3, 32, 16, 0, tr("..."));

        if (CManageViewSigleton::GetInstance()->getThemeType() == 1) {
            textPen.setColor(QColor("#414D68"));
        } else {
            textPen.setColor(QColor("#C0C6D4"));
        }

        painter.setPen(textPen);
        ft.setPixelSize(14);
        painter.setFont(ft);
        painter.drawText(26, 6, 38, 22, 0, tr("Color"));
    }
}

void TextColorButton::setColor(QColor color)
{
    m_isMultColorSame = true;
    m_color = color;
    update();
}

void TextColorButton::setColorIndex(int index)
{
    m_color = colorIndexOf(index);
    update();
}

void TextColorButton::enterEvent(QEvent *)
{
    if (!m_isHover) {
        m_isHover = true;
        update();
    }
}

void TextColorButton::leaveEvent(QEvent *)
{
    if (m_isHover) {
        m_isHover = false;
        update();
    }
}

void TextColorButton::mousePressEvent(QMouseEvent * )
{
    m_isChecked = !m_isChecked;
    update();

    btnCheckStateChanged(m_isChecked);
}

void TextColorButton::resetChecked()
{
    m_isChecked = false;
    update();
}

void TextColorButton::setIsMultColorSame(bool isMultColorSame)
{
    m_isMultColorSame = isMultColorSame;
}
