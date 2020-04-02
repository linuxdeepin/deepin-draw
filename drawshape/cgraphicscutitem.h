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
#ifndef CGRAPHICSCUTITEM_H
#define CGRAPHICSCUTITEM_H

#include "cgraphicsitem.h"
#include "cbuttonrect.h"

class CSizeHandleRect;
class QGraphicsProxyWidget;

class CGraphicsCutItem : public CGraphicsItem
{

public:
    explicit CGraphicsCutItem(CGraphicsItem *parent = nullptr);
    explicit CGraphicsCutItem(const QRectF &rect, CGraphicsItem *parent = nullptr);
    explicit CGraphicsCutItem(qreal x, qreal y, qreal w, qreal h, CGraphicsItem *parent = nullptr);
    virtual ~CGraphicsCutItem()  Q_DECL_OVERRIDE;
    virtual int  type() const Q_DECL_OVERRIDE;
    virtual void setRect(const QRectF &rect);
    virtual QRectF rect() const Q_DECL_OVERRIDE;
    virtual QRectF boundingRect() const Q_DECL_OVERRIDE;
    virtual void resizeTo(CSizeHandleRect::EDirection dir, const QPointF &point ) Q_DECL_OVERRIDE;
    virtual void resizeTo(CSizeHandleRect::EDirection dir, const QPointF &point, bool bShiftPress, bool bAltPress ) Q_DECL_OVERRIDE;

    void doChangeType(int);
    void doChangeSize(int, int);


    bool isFreeMode() const;
    void setIsFreeMode(bool isFreeMode);

    virtual void duplicate(CGraphicsItem *item) Q_DECL_OVERRIDE;

protected:
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) Q_DECL_OVERRIDE;
    virtual QVariant itemChange(GraphicsItemChange change, const QVariant &value) Q_DECL_OVERRIDE;
    virtual void updateGeometry() Q_DECL_OVERRIDE;

private:
    /**
     * @brief initRect 初始化矩形的属性和边框小方块
     */
    void initRect();
    void drawFourConner(QPainter *painter);
    void drawTrisectorRect(QPainter *painter);
    void showControlRects(bool);

private:
    QPointF m_topLeftPoint; //左上角的点
    QPointF m_bottomRightPoint; //右下角的点
    QRectF m_originalRect;
    bool m_isFreeMode;

};

#endif // CGRAPHICSCUTITEM_H
