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
    explicit ColorLabel(DWidget *parent = nullptr);
    ~ColorLabel();

    //h∈(0, 360), s∈(0, 1), v∈(0, 1)
    QColor getColor(qreal h, qreal s, qreal v);
    void setHue(int hue);

    /**
     * @brief pickColor 获取鼠标位置颜色
     * @param pos　位置
     * @param picked　是否获取
     */
    void pickColor(QPoint pos, bool picked = false);
//    /**
//     * @brief getPickedColor　获取颜色
//     * @return
//     */
//    QColor getPickedColor();
//    /**
//     * @brief setPickColor　设置颜色
//     * @param picked　是否设置
//     */
//    void setPickColor(bool picked);

signals:
    /**
     * @brief clicked　鼠标点击信号
     */
    void clicked();
    /**
     * @brief pickedColor　获取颜色信号
     * @param color　颜色
     */
    void pickedColor(QColor color);
    /**
     * @brief signalPreViewColor　颜色预览信号
     * @param color　颜色
     */
    void signalPreViewColor(QColor color);

protected:
    void paintEvent(QPaintEvent *);
    /**
     * @brief enterEvent　鼠标进入事件
     * @param e　事件
     */
    void enterEvent(QEvent *e);
    /**
     * @brief leaveEvent　鼠标离开事件
     * @param e　事件
     */
    void leaveEvent(QEvent *e);
    /**
     * @brief mousePressEvent　鼠标点击事件
     * @param e　鼠标事件
     */
    void mousePressEvent(QMouseEvent *e);
    /**
     * @brief mouseMoveEvent　鼠标移动事件
     * @param e　鼠标事件
     */
    void mouseMoveEvent(QMouseEvent *e);
    /**
     * @brief mouseReleaseEvent　鼠标弹起事件
     * @param e　鼠标事件
     */
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
