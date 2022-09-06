// SPDX-FileCopyrightText: 2020 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef CGRAPHICSPROXYWIDGET_H
#define CGRAPHICSPROXYWIDGET_H

#include <QGraphicsProxyWidget>
#include <QSet>

class CGraphicsItem;
class CGraphicsProxyWidget : public QGraphicsProxyWidget
{
public:
    CGraphicsProxyWidget(QGraphicsItem *parent = nullptr, Qt::WindowFlags wFlags = Qt::WindowFlags());

    CGraphicsItem *parentDrawItem();

    bool isFocusFriendWgt(QWidget *w);

protected:
    void focusInEvent(QFocusEvent *event) override;
    void focusOutEvent(QFocusEvent *event) override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    QVariant itemChange(GraphicsItemChange doChange, const QVariant &value) override;

private:
    QSet<QWidget *> _friendWidgets;
};

#endif // CGRAPHICSPROXYWIDGET_H
