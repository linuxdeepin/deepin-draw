/*
 * Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
 *
 * Author:     Ji XiangLong <jixianglong@uniontech.com>
 *
 * Maintainer: WangYu <wangyu@uniontech.com>
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
#include "ccolorpickwidget.h"
#include "colorpanel.h"
#include "application.h"

//#include "application.h"
//#include "mainwindow.h"
//#include "toptoolbar.h"

CColorPickWidget::CColorPickWidget(QWidget *parent)
    : DArrowRectangle(ArrowTop, FloatWidget, parent)
{
    drawApp->setWidgetAccesibleName(this, "ColorPickWidget");
    this->setWindowFlag(Qt::FramelessWindowHint);
    this->setWindowFlag(Qt::/*Popup*/Widget);
    this->setAttribute(Qt::WA_TranslucentBackground, true);
    this->setArrowWidth(18);
    this->setArrowHeight(10);
    m_colorPanel = new ColorPanel(this);
    m_colorPanel->setFocusPolicy(Qt::NoFocus);
    this->setFocusPolicy(Qt::NoFocus);
    this->setContent(m_colorPanel);
    this->hide();

    //connect(m_colorPanel, &ColorPanel::colorChanged, this, &CColorPickWidget::colorChanged);
    connect(m_colorPanel, &ColorPanel::colorChanged, this, [ = ](const QColor & color, EChangedPhase phase) {
        qDebug() << "color ===== " << color << "phase = " << phase;
        if (_color != color || _phase != phase) {
            _color = color;
            _phase = phase;
            emit colorChanged(color, phase);
        }
    });
}

QColor CColorPickWidget::color()
{
    return _color;
}

ColorPanel *CColorPickWidget::colorPanel()
{
    return m_colorPanel;
}

QWidget *CColorPickWidget::caller()
{
    return _caller;
}

void CColorPickWidget::setCaller(QWidget *pCaller)
{
    _caller = pCaller;
}

void CColorPickWidget::show(int x, int y)
{
    if (parentWidget() == nullptr || isWindowType()) {
        return DArrowRectangle::show(x, y);
    }
    QPoint pos = this->parentWidget()->mapFromGlobal(QPoint(x, y));
    DArrowRectangle::show(pos.x(), pos.y());
}

void CColorPickWidget::setColor(const QColor &c)
{
    m_colorPanel->setColor(c, false);
    _color = c;
    _phase = EChanged;
}

void CColorPickWidget::setTheme(int theme)
{
    m_colorPanel->setTheme(theme);
}
void CColorPickWidget::mousePressEvent(QMouseEvent *event)
{
    event->accept();
}
