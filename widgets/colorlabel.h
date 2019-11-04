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
#ifndef COLORLABEL_H
#define COLORLABEL_H

#include <DLabel>
#include <DWidget>

#include <QPaintEvent>
#include <QEnterEvent>
#include <QMouseEvent>
#include <QCursor>

DWIDGET_USE_NAMESPACE

class ColorLabel : public DLabel
{
    Q_OBJECT
public:
    ColorLabel(DWidget *parent = 0);
    ~ColorLabel();

    //h∈(0, 360), s∈(0, 1), v∈(0, 1)
    QColor getColor(qreal h, qreal s, qreal v);
    void setHue(int hue);

    void pickColor(QPoint pos, bool picked = false);
    QColor getPickedColor();
    void setPickColor(bool picked);

signals:
    void clicked();
    void pickedColor(QColor color);
    void signalPreViewColor(QColor color);

protected:
    void paintEvent(QPaintEvent *);
    void enterEvent(QEvent *e);
    void leaveEvent(QEvent *e);
    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);

private:
    //calculate the color's rgb value in pos.
    QColor calColorAtPoint(QPointF pos);
    QCursor m_lastCursor;
    int m_hue =  0;
    bool m_workToPick;
    bool m_picking;
    bool m_pressed;
    QColor m_pickedColor;
    QPoint m_clickedPos;
    QPoint m_tipPoint;
};

#endif // COLORLABEL_H
