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
#ifndef CNUMBERSLIDERWIDGET_H
#define CNUMBERSLIDERWIDGET_H


#include <QWidget>
#include <QSlider>
#include <QLabel>
#include <QLineEdit>

#include "globaldefine.h"
#include "adaptivedef.h"

#ifdef USE_DTK
#include <DLineEdit>
#include <DSlider>
DWIDGET_USE_NAMESPACE
#endif
class DRAWLIB_EXPORT NumberSlider : public QWidget
{
    Q_OBJECT
public:
    explicit NumberSlider(QWidget *parent = nullptr);
    explicit NumberSlider(const QString &text, QWidget *parent = nullptr);

    void setValue(int value);
    int  value() const;

    void setRange(int min, int max);
    int min()const;
    int max()const;

    void setText(const QString &str);
    QString text()const;

    void setTextVisiable(bool b);
    bool isTextVisiable()const;

    void setFlagDiameter(uint diameter);
    uint flagDiameter()const;

    void setTickInterval(int t);
    void setPix(QPixmap pix);

signals:
    void valueChanged(int value, EChangedPhase phase);

private:
    PRIVATECLASS(NumberSlider)
};


#endif // CNUMBERSLIDERWIDGET_H
