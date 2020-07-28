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
#ifndef CCOLORPICKWIDGET_H
#define CCOLORPICKWIDGET_H

#include <QWidget>
#include <DArrowRectangle>

DWIDGET_USE_NAMESPACE

class ColorPanel;
class CColorPickWidget : public DArrowRectangle
{
    Q_OBJECT
public:
    explicit CColorPickWidget(QWidget *parent = nullptr);

public slots:

    void setColor(const QColor &c);

    void setTheme(int theme);

signals:
    void colorChanged(const QColor &color, bool preview);

private:
    ColorPanel *m_colorPanel = nullptr;

    QColor _color;
};

#endif // CCOLORPICKWIDGET_H
