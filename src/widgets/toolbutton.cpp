/*
* Copyright (C) 2019 ~ 2019 Deepin Technology Co., Ltd.
*
* Author: Tang Peng <tangpeng@uniontech.com>
*
* Maintainer: Tang Peng <tangpeng@uniontech.com>
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

#include "toolbutton.h"
#include "frame/cviewmanagement.h"
#include <QPainter>
#include <QStylePainter>
#include <QStyleOptionButton>
#include <qtoolbutton.h>

ToolButton::ToolButton(QWidget *parent) : DPushButton(parent)
{

}

void ToolButton::setText(const QString &text)
{
    //控件设置需要绘制的文字
    m_text = text;
}

void ToolButton::setIcon(const QIcon &icon)
{
    //控件设置需要绘制的图片
    m_icon = icon;
}

void ToolButton::paintEvent(QPaintEvent *e)
{
    Q_UNUSED(e)

    QStylePainter p(this);
    QStyleOptionButton option;
    initStyleOption(&option);

    QPainter painter(this);
    //绘制背景
    painter.save();
    //非禁用状态绘制背景
    if (option.state & QStyle::State_Enabled) {
        if (option.state & QStyle::State_MouseOver) {
            painter.setPen(Qt::NoPen);
            //获取系统主题颜色
            QColor hovertColor(option.palette.highlight().color());
            painter.setBrush(hovertColor);
            painter.drawRect(this->rect());
        }
    }
    painter.restore();

    if (CManageViewSigleton::GetInstance()->getThemeType() == 1) {
        //控件禁用样式
        if (!(option.state & QStyle::State_Enabled)) {
            painter.setPen(QColor("#9C9C9C"));
        }

        //鼠标悬停画笔颜色
        else if (option.state & QStyle::State_MouseOver) {
            painter.setPen(QColor(Qt::white));
        }

        //鼠标按下画笔颜色
        else if (option.state & QStyle::State_Sunken) {
            painter.setPen(QColor("#99cdff"));
        } else {
            painter.setPen(QColor("#343434"));
        }

    } else {

        //控件禁用样式
        if (!(option.state & QStyle::State_Enabled)) {
            painter.setPen(QColor("#8D8D8D"));
        }

        //鼠标悬停画笔颜色
        else if (option.state & QStyle::State_MouseOver) {
            painter.setPen(QColor(Qt::white));
        }

        //鼠标按下画笔颜色
        else if (option.state & QStyle::State_Sunken) {
            painter.setPen(QColor("#99cdff"));
        } else {

            painter.setPen(QColor(Qt::white));
        }
    }


    // 绘制图片
    painter.save();
    m_icon.paint(&painter, QRect(10, -6, 45, 45));
    painter.restore();

    //绘制文字
    painter.save();
    QFont ft;
    ft.setPixelSize(14);
    painter.setFont(ft);
    painter.drawText(50, 5, 80, 22, 0, m_text);
    painter.restore();
}










