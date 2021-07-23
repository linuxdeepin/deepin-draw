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
#include <DPlatformWindowHandle>
#include <DWindowManagerHelper>
CColorPickWidget::CColorPickWidget(QWidget *parent)
    : DArrowRectangle(ArrowTop, FloatWidget, parent)
{
    setWgtAccesibleName(this, "ColorPickWidget");
    this->setWindowFlag(Qt::FramelessWindowHint);
    this->setWindowFlag(Qt::/*Popup*/Popup);
    this->setAttribute(Qt::WA_TranslucentBackground, true);
    this->setArrowWidth(18);
    this->setArrowHeight(10);
    m_colorPanel = new ColorPanel(this);
    m_colorPanel->setFocusPolicy(Qt::NoFocus);
    this->setFocusPolicy(Qt::NoFocus);
    //this->setContent(m_colorPanel);
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

//    connect(DWindowManagerHelper::instance(), &DWindowManagerHelper::hasBlurWindowChanged, this, [ = ]() {
//        bool hasBlur = DWindowManagerHelper::instance()->hasBlurWindow();

//        if (hasBlur) {
//            m_colorPanel->setWindowFlag(Qt::Widget);
//            m_colorPanel->setParent(this);
//            setContent(m_colorPanel);
//            this->setContent(m_colorPanel);
//        } else {
//            m_colorPanel->setWindowFlag(Qt::Popup);
//            this->hide();
//        }
//    });
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

void CColorPickWidget::setExpandWidgetVisble(bool visble)
{
    colorPanel()->setExpandWidgetVisble(visble);
}

void CColorPickWidget::show(int x, int y)
{
    //if (parentWidget() == nullptr || isWindowType())
    {
        //DPlatformWindowHandle hander(this);
        //hander.setEnableBlurWindow(false);
        //QPainterPath path; path.addRect(this->rect());
        //hander.setClipPath(path);
        //hander.setFrameMask(QRegion(0, 0, width(), height()));
        //this->setArrowWidth(0);
        //this->setArrowHeight(0);

        if (DWindowManagerHelper::instance()->hasBlurWindow()) {
            m_colorPanel->setWindowFlags(Qt::Widget);
            m_colorPanel->setParent(this);
            setContent(m_colorPanel);
            return DArrowRectangle::show(x, y);
        } else {
            setContent(nullptr);
            this->hide();
            qWarning() << "show getContent======== " << getContent();
            //DPlatformWindowHandle hander(m_colorPanel);
            m_colorPanel->setWindowFlag(Qt::Popup);
            m_colorPanel->move(x - m_colorPanel->width() / 2, y);
            m_colorPanel->show();
            return;
        }

        //Dtk::Widget::DPlatformWindowHandle
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
    //event->accept();

    return DArrowRectangle::mousePressEvent(event);
}

bool CColorPickWidget::event(QEvent *e)
{
//    if (e->type() == QEvent::MouseMove) {
//        return  true;
//    }

    if (e->type() == QEvent::Hide) {
        m_colorPanel->hide();
    } else if (e->type() == QEvent::Show) {
        m_colorPanel->show();
    }

    return DArrowRectangle::event(e);
}
