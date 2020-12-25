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
#ifndef COLORSLIDER_H
#define COLORSLIDER_H

//#include <QSlider>
#include <QPaintEvent>
#include <QSlider>


class ColorSlider : public QSlider
{
    Q_OBJECT
public:
    explicit ColorSlider(QWidget *parent = nullptr);
    ~ColorSlider();

    //h∈(0, 360), s∈(0, 1), v∈(0, 1)
    QColor getColor(qreal h, qreal s, qreal v);

protected:
    void paintEvent(QPaintEvent *ev);

private:
    int m_value;
    QImage m_backgroundImage;
};

#endif // COLORSLIDER_H
