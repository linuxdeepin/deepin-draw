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

#include "toolbutton.h"
#include "frame/cviewmanagement.h"
#include <QPainter>
#include <QStylePainter>
#include <QStyleOptionButton>
#include <qtoolbutton.h>
#include <DGuiApplicationHelper>

DGUI_USE_NAMESPACE

ToolButton::ToolButton(QWidget *parent) : DPushButton(parent)
{

}

//void ToolButton::setText(const QString &text)
//{
//    //控件设置需要绘制的文字
//    m_text = text;
//}

//void ToolButton::setIcon(const QIcon &icon)
//{
//    //控件设置需要绘制的图片
//    m_icon = icon;
//}

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

    painter.setPen(getPen(option));
//    if (CManageViewSigleton::GetInstance()->getThemeType() == 1) {
//        //控件禁用样式
//        if (!(option.state & QStyle::State_Enabled)) {
//            painter.setPen(QColor("#9C9C9C"));
//        }

//        //鼠标悬停画笔颜色
//        else if (option.state & QStyle::State_MouseOver) {
//            painter.setPen(QColor(Qt::white));
//        }

//        //鼠标按下画笔颜色
//        else if (option.state & QStyle::State_Sunken) {
//            painter.setPen(QColor("#99cdff"));
//        } else {
//            painter.setPen(QColor("#343434"));
//        }

//    } else {

//        //控件禁用样式
//        if (!(option.state & QStyle::State_Enabled)) {
//            painter.setPen(QColor("#8D8D8D"));
//        }

//        //鼠标悬停画笔颜色
//        else if (option.state & QStyle::State_MouseOver) {
//            painter.setPen(QColor(Qt::white));
//        }

//        //鼠标按下画笔颜色
//        else if (option.state & QStyle::State_Sunken) {
//            painter.setPen(QColor("#99cdff"));
//        } else {

//            painter.setPen(QColor(Qt::white));
//        }
//    }

    painter.save();
    painter.translate(20, 0);

    // 绘制图片
    icon().paint(&painter, QRect(-14, (height() - 45) / 2, 45, 45));

    //绘制文字
    QFont ft = QFont();
    painter.setFont(ft);
    QRect textRect(28, 5, width() - 28, height() - 10);
    painter.drawText(textRect, text(), QTextOption(Qt::AlignVCenter | Qt::AlignLeft));
    painter.restore();
}

QPen ToolButton::getPen(const QStyleOptionButton option)
{
    QPen pen;
    if (DGuiApplicationHelper::instance()->themeType() == 1) {
        //控件禁用样式
        if (!(option.state & QStyle::State_Enabled)) {
            pen = QPen(QColor("#9C9C9C"));
        }

        //鼠标悬停画笔颜色
        else if (option.state & QStyle::State_MouseOver) {
            pen = QPen(QColor(Qt::white));
        }

        //鼠标按下画笔颜色
        else if (option.state & QStyle::State_Sunken) {
            pen = QPen(QColor("#99cdff"));
        } else {
            pen = QPen(QColor("#343434"));
        }

    } else {

        //控件禁用样式
        if (!(option.state & QStyle::State_Enabled)) {
            pen = QPen(QColor("#8D8D8D"));
        }

        //鼠标悬停画笔颜色
        else if (option.state & QStyle::State_MouseOver) {
            pen = QPen(QColor(Qt::white));
        }

        //鼠标按下画笔颜色
        else if (option.state & QStyle::State_Sunken) {
            pen = QPen(QColor("#99cdff"));
        } else {

            pen = QPen(QColor(Qt::white));
        }
    }
    return pen;
}










