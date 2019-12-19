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
#ifndef TEXTCOLORBUTTON_H
#define TEXTCOLORBUTTON_H

#include <DPushButton>
#include <QPainter>
#include <QPaintEvent>

DWIDGET_USE_NAMESPACE

class TextColorButton : public DPushButton
{
    Q_OBJECT
public:
    TextColorButton(DWidget *parent = nullptr);
    ~TextColorButton();

    void setColor(QColor color);
    void setColorIndex(int index);
    void updateConfigColor();
    void resetChecked();

signals:
    void btnCheckStateChanged(bool show);

protected:
    void paintEvent(QPaintEvent *);
    void enterEvent(QEvent *);
    void leaveEvent(QEvent *);
    void mousePressEvent(QMouseEvent * );

private:
    QColor m_color;
    bool m_isHover;
    bool m_isChecked;

};

#endif // TEXTCOLORBUTTON_H
