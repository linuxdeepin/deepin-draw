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
#ifndef CGRAPHICSPROXYWIDGET_H
#define CGRAPHICSPROXYWIDGET_H

#include <globaldefine.h>
#include <QGraphicsProxyWidget>

class PageItem;
class DRAWLIB_EXPORT ProxyWidgetItem : public QGraphicsProxyWidget
{
public:
    ProxyWidgetItem(PageItem *parent = nullptr, Qt::WindowFlags wFlags = Qt::WindowFlags());

    PageItem *parentDrawItem();

protected:
    bool event(QEvent *event) override;
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;
};

class DRAWLIB_EXPORT ProxyTextWidgetItem: public ProxyWidgetItem
{
public:
    ProxyTextWidgetItem(PageItem *parent = nullptr, Qt::WindowFlags wFlags = Qt::WindowFlags());

protected:
    void focusInEvent(QFocusEvent *event) override;
    void focusOutEvent(QFocusEvent *event) override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    QVariant itemChange(GraphicsItemChange doChange, const QVariant &value) override;
};

#endif // CGRAPHICSPROXYWIDGET_H
