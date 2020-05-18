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

    //m_color = CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getFillColor();
}

void BigColorButton::updateConfigColor()
{
    m_isMultColorSame = true;
    QColor configColor = CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getFillColor();

    if (m_color == configColor) {
        update();
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

    paintLookStyle(&painter, !m_isMultColorSame);
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

void BigColorButton::paintLookStyle(QPainter *painter, bool isMult)
{
    //const QColor borderColor(77, 82, 93, int(1.0 * 255));
    //const QColor borderColor(255, 255, 255, int(0.1 * 255));
    const QColor borderColor = (isMult || m_color.alpha() == 0) ? QColor(77, 82, 93, int(0.8 * 255)) : QColor(255, 255, 255, int(0.1 * 255));
    painter->save();
    painter->setRenderHints(QPainter::Antialiasing);

    const QSizeF btSz = QSizeF(20, 20);

    QPointF topLeft = QPointF(4, 8);

    QPen pen(painter->pen());

    bool   darkTheme = (CManageViewSigleton::GetInstance()->getThemeType() == 2);
    QColor penColor  = darkTheme ? borderColor : QColor(0, 0, 0, int(0.1 * 255));

    pen.setColor(penColor);

    pen.setWidthF(1.5);    //多加0.5宽度弥补防走样的误差

    painter->setPen(pen);

    painter->translate(topLeft);

    QPainterPath path;

    QRectF outerct(QPointF(0, 0), btSz);
    QRectF inerrct(QPointF(3, 3), btSz - QSizeF(2 * 3, 2 * 3));

    path.addRoundedRect(outerct, 8, 8);

    //填充色的设置(多选颜色冲突时(图元填充色不一致那么不设置填充色在后续绘制一条斜线))
    painter->setBrush((isMult || m_color.alpha() == 0) ?
                      (darkTheme ? QColor(8, 15, 21, int(0.7 * 255)) : QColor(0, 0, 0, int(0.05 * 255))) : m_color);

    painter->drawPath(path);

    //如果多选颜色有冲突(isMult为true时)那么就绘制"..."
    if (isMult) {
        painter->save();
        //painter->setPen(darkTheme ? QColor("#C0C6D4") : QColor("#414D68"));
        QColor cp = darkTheme ? QColor("#C5CFE0") : QColor("#000000");
        cp.setAlpha(darkTheme ? int(0.8 * 255) : int(0.2 * 255));
        painter->setPen(cp);
        QFont ft;
        ft.setPixelSize(14);
        painter->setFont(ft);
        painter->drawText(outerct.translated(0, -painter->fontMetrics().height() / 4), tr("..."), QTextOption(Qt::AlignCenter));
        painter->restore();
    } else {
        //颜色没有冲突 如果颜色是透明的要绘制一条斜线表示没有填充色
        if (m_color.alpha() == 0) {
            QPen pen(QColor(255, 67, 67, 153));
            pen.setWidthF(2.0);
            painter->setPen(pen);
            painter->drawLine(QLineF(inerrct.bottomLeft(), inerrct.topRight()));
        }
    }

    painter->restore();

    //绘制常量文字("填充")
    painter->save();
    painter->setPen(darkTheme ? QColor("#C0C6D4") : QColor("#414D68"));
    QFont ft;
    ft.setPixelSize(14);
    painter->setFont(ft);
    painter->drawText(32, 6, 28, 22, 0, tr("Fill"));
    painter->restore();
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
