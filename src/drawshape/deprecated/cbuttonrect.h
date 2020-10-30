/*
 * Copyright (C) 2019 ~ %YEAR% Deepin Technology Co., Ltd.
 *
 * Author:     Renran
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

#ifndef CBUTTONRECT_H
#define CBUTTONRECT_H

#include <QGraphicsRectItem>
#include <QTextOption>



class CButtonRect : public QGraphicsRectItem
{
public:

    enum EButtonType {OKButton, CancelButton, NoneButton};

    CButtonRect(QGraphicsItem *parent,  EButtonType type);
    ~CButtonRect() override;
//    CButtonRect::EButtonType buttonType() const;
    void move(qreal x, qreal y);
    bool hitTest(const QPointF &point);
    QRectF boundingRect() const override;

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;
//    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
//    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
private:
    EButtonType m_buttonType;
    QString m_text;
    QColor m_backColor;

};

#endif // CBUTTONRECT_H
