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
#ifndef CSVGLABEL_H
#define CSVGLABEL_H

#include <QLabel>
#include <QSvgRenderer>
//#ifdef USE_DTK
//DWIDGET_USE_NAMESPACE
//#endif
class QWidget;

class CSvgLabel : public QLabel
{
    Q_OBJECT
public:
    CSvgLabel(const QString &str = QString(), QWidget *parent = nullptr);

protected:
    virtual void paintEvent(QPaintEvent *event) override;

private:
    QSvgRenderer m_svgRender;
};

#endif // CSVGLABEL_H
