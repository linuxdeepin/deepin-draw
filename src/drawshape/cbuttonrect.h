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
    ~CButtonRect() Q_DECL_OVERRIDE;
//    CButtonRect::EButtonType buttonType() const;
    void move(qreal x, qreal y);
    bool hitTest(const QPointF &point);
    QRectF boundingRect() const Q_DECL_OVERRIDE;

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) Q_DECL_OVERRIDE;
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *event) Q_DECL_OVERRIDE;
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) Q_DECL_OVERRIDE;
//    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event) Q_DECL_OVERRIDE;
//    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) Q_DECL_OVERRIDE;
private:
    EButtonType m_buttonType;
    QString m_text;
    QColor m_backColor;

};

#endif // CBUTTONRECT_H
