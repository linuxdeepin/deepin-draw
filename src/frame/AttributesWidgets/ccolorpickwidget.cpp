/*
* Copyright (C) 2019 ~ 2019 Deepin Technology Co., Ltd.
*
* Author: Ji Xianglong<jixianglong@uniontech.com>
*
* Maintainer: Ji Xianglong <jixianglong@uniontech.com>
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program. If not, see <http://www.gnu.org/licenses/>.
*/
#include "ccolorpickwidget.h"
#include "colorpanel.h"

//#include "application.h"
//#include "mainwindow.h"
//#include "toptoolbar.h"

CColorPickWidget::CColorPickWidget(QWidget *parent)
    : DArrowRectangle(ArrowTop, FloatWidget, parent)
{
    this->setWindowFlag(Qt::FramelessWindowHint);
    this->setWindowFlag(Qt::Popup);
    this->setAttribute(Qt::WA_TranslucentBackground, true);
    this->setArrowWidth(18);
    this->setArrowHeight(10);
    m_colorPanel = new ColorPanel(this);
    this->setContent(m_colorPanel);
    this->hide();

    //connect(m_colorPanel, &ColorPanel::colorChanged, this, &CColorPickWidget::colorChanged);
    connect(m_colorPanel, &ColorPanel::colorChanged, this, [ = ](const QColor & color, EChangedPhase phase) {
        qDebug() << "color ===== " << color << "phase = " << phase;        static QColor s_c;
        static EChangedPhase s_phase = EChangedUpdate;
        if (s_c != color || s_phase != phase) {
            s_c = color;
            s_phase = phase;
            emit colorChanged(color, phase);
        }
    });
}

void CColorPickWidget::setColor(const QColor &c)
{
    m_colorPanel->setColor(c, false);
}

void CColorPickWidget::setTheme(int theme)
{
    m_colorPanel->setTheme(theme);
}
