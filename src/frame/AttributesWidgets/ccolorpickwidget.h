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
#ifndef CCOLORPICKWIDGET_H
#define CCOLORPICKWIDGET_H

#include <QWidget>
#include <DArrowRectangle>

#include "globaldefine.h"

DWIDGET_USE_NAMESPACE

class ColorPanel;
class CColorPickWidget : public DArrowRectangle
{
    Q_OBJECT
public:
    explicit CColorPickWidget(QWidget *parent = nullptr);

    QColor color();

    ColorPanel *colorPanel();

    QWidget *caller();

    void setCaller(QWidget *pCaller);

    void setExpandWidgetVisble(bool visble);

    void show(int x, int y) override;

public slots:

    void setColor(const QColor &c);

    void setTheme(int theme);

signals:
    void colorChanged(const QColor &color, EChangedPhase phase);

protected:
    void mousePressEvent(QMouseEvent *event) override;

    bool event(QEvent *e) override;

private:
    ColorPanel *m_colorPanel = nullptr;

    QColor        _color;

    EChangedPhase _phase = EChangedUpdate;

    QWidget      *_caller = nullptr;
};

#endif // CCOLORPICKWIDGET_H
