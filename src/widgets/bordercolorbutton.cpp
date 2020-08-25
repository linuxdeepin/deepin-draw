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
#include "application.h"
#include "ccolorpickwidget.h"

//const qreal COLOR_RADIUS = 4;
//const int BTN_RADIUS = 8;
//const QPoint CENTER_POINT = QPoint(12, 12);

BorderColorButton::BorderColorButton(DWidget *parent)
    : DPushButton(parent)
    , m_isHover(false)
    , m_isChecked(false)
    , m_isMultColorSame(false)
{
    setCheckable(false);
    setButtonText(tr("Stroke"));
    //m_color = CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getLineColor();
    this->setFocusPolicy(Qt::StrongFocus);
}

void BorderColorButton::updateConfigColor()
{
    m_isMultColorSame = true;
    QColor configColor = CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getLineColor();

    if (m_color == configColor) {
        update();
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

    //绘制looking
    paintLookStyle(&painter, !m_isMultColorSame);
}

void BorderColorButton::paintLookStyle(QPainter *painter, bool isMult)
{
    //const QColor borderColor(77, 82, 93, int(1.0 * 255));
    //const QColor borderColor(255, 255, 255, int(0.1 * 255));
    const QColor borderColor = (isMult || m_color.alpha() == 0) ? QColor(77, 82, 93, int(0.8 * 255)) : QColor(255, 255, 255, int(0.1 * 255));
    painter->save();
    painter->setRenderHints(QPainter::Antialiasing);

    const QSizeF btSz = QSizeF(20, 20);

    QPointF topLeft = QPointF(4, 8);;

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
    path.addRoundedRect(inerrct, 5, 5);

    //线条的颜色用path的填充色来表示(如果是选中了多个图元那么有默认的颜色(该默认颜色与主题相关))
    painter->setBrush((isMult || m_color.alpha() == 0) ?
                      (darkTheme ? QColor(8, 15, 21, int(0.7 * 255)) : QColor(0, 0, 0, int(0.05 * 255))) : m_color);

    painter->drawPath(path);

    //如果颜色是透明的要绘制一条斜线表示没有填充色
    if (!isMult && m_color.alpha() == 0) {
        QPen pen(QColor(255, 67, 67, 153));
        pen.setWidthF(2.0);
        painter->setPen(pen);
        painter->drawLine(QLineF(inerrct.bottomLeft(), inerrct.topRight()));
    }

    painter->restore();

    //绘制常量文字("描边")
    painter->save();
    painter->setPen(darkTheme ? QColor("#C0C6D4") : QColor("#414D68"));
    QFont ft;
    ft.setPixelSize(14);
    painter->setFont(ft);
    painter->drawText(32, 6, TextWidth, 22, 1, m_text);
    painter->restore();
}

void BorderColorButton::setColor(QColor color, EChangedPhase phase)
{
    //if (color != m_color)
    {
        m_isMultColorSame = true;
        m_color = color;
        update();

        emit colorChanged(color, phase);
    }
}

QColor BorderColorButton::getColor() const
{
    return m_color;
}

void BorderColorButton::setColorIndex(int index)
{
    m_isMultColorSame = true;
    m_color = colorIndexOf(index);
    update();
}

QColor BorderColorButton::getColorByIndex() const
{
    return m_color;
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

bool BorderColorButton::getIsMultColorSame() const
{
    return m_isMultColorSame;
}

void BorderColorButton::setButtonText(QString text)
{
    QFontMetrics fontMetrics(font());
    m_textWidth = fontMetrics.width(text);
    setFixedSize(35 + TextWidth, 32);
    m_text = text;
}

QString BorderColorButton::getButtonText() const
{
    return m_text;
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

void BorderColorButton::mousePressEvent(QMouseEvent *)
{
    m_isChecked = !m_isChecked;

    update();

    // 显示颜色提取窗口
    QPoint btnPos = mapToGlobal(QPoint(0, 0));
    QPoint pos(btnPos.x() + 14,
               btnPos.y() + this->height());

    CColorPickWidget *pColorPick = dApp->colorPickWidget(true, this);

    pColorPick->setColor(this->getColor());

    connect(pColorPick, &CColorPickWidget::colorChanged, this, [ = ](const QColor & color, EChangedPhase phase) {
        this->setColor(color, phase);
    });
    pColorPick->show(pos.x(), pos.y());
}
