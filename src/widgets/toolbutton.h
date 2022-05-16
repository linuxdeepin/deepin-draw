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

#ifndef TOOLBUTTON_H
#define TOOLBUTTON_H

#include <QWidget>
#include <DPushButton>

DWIDGET_USE_NAMESPACE

enum ToolButonStyle {
    MENU_STYLE,
    BUTTON_STYLE
};

class ToolButton : public QPushButton
{
    Q_OBJECT
public:

    /**
     * @description: ToolButton 构造函数
    */
    explicit ToolButton(QWidget *parent = nullptr, ToolButonStyle style = MENU_STYLE);

    void setBtnStyle(ToolButonStyle style);
    void setShowText(bool bShowText);

    void setAlignment(Qt::Alignment alignment = Qt::AlignLeft);
protected:
    /**
     * @description: paintEvent 重新绘制控件的样式
    */
    void paintEvent(QPaintEvent *e) override;

    QPen getPen(const QStyleOptionButton option);
    QSize sizeHint() const override;

private:
    bool m_bShowText;
    ToolButonStyle m_style;
    Qt::Alignment m_alignment;
};

#endif // TOOLBUTTON_H
