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
#ifndef CGRAPHICSROTATEANGLEITEM_H
#define CGRAPHICSROTATEANGLEITEM_H

#include <QGraphicsRectItem>
#include <QFont>

class CGraphicsRotateAngleItem : public QGraphicsRectItem
{
public:
    explicit CGraphicsRotateAngleItem(qreal rotateAngle, qreal scale, QGraphicsItem *parent = nullptr);
    explicit CGraphicsRotateAngleItem(const QRectF &rect, qreal rotateAngle, QGraphicsItem *parent = nullptr);

    void updateRotateAngle(qreal rotateAngle);
protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) Q_DECL_OVERRIDE;

private:
    qreal m_rotateAngle;
    qreal m_width;
    qreal m_height;
    qreal m_fontSize;
    QFont m_textFont;
};

#endif // CGRAPHICSROTATEANGLEITEM_H
