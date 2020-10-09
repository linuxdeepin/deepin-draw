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
    ~CGraphicsCutItem()  override;
    int            type() const override;
    virtual void   setRect(const QRectF &rect);
    virtual QRectF rect() const override;
    virtual QRectF boundingRect() const override;
    virtual void   resizeTo(CSizeHandleRect::EDirection dir, const QPointF &point) override;
    virtual void   move(QPointF beginPoint, QPointF movePoint) override;
    virtual CSizeHandleRect::EDirection hitTest(const QPointF &point) const override;

    /**
     * @brief getHighLightPath 获取高亮path
     * @return
     */
    virtual QPainterPath getHighLightPath() override;

    /**
     * @brief isPosPenetrable 某一位置在图元上是否是可穿透的（透明的）(基于inSideShape和outSideShape)
     * @param posLocal 该图元坐标系的坐标位置
     */
    virtual bool isPosPenetrable(const QPointF &posLocal) override;

    void doChangeType(int);
    int  getCutType() const;
    void doChangeSize(int, int);


    bool isFreeMode() const;
    void setIsFreeMode(bool isFreeMode);

    void duplicate(CGraphicsItem *item) override;


    void  resizeCutSize(CSizeHandleRect::EDirection dir, const QPointF &prePoint, const QPointF &point,
                        QPointF *outAcceptPos = nullptr);

    qreal getWHRadio();

    /**
     * @brief nodes  获取操作缩放节点
     */
    Handles nodes();

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;
    void updateHandlesGeometry() override;

private:
    /**
     * @brief initRect 初始化矩形的属性和边框小方块
     */
    void initHandle() override;
    void drawFourConner(QPainter *painter);
    void drawTrisectorRect(QPainter *painter);
    void showControlRects(bool);

private:
    QPointF m_topLeftPoint; //左上角的点
    QPointF m_bottomRightPoint; //右下角的点
    QRectF  m_originalRect;
    bool    m_isFreeMode;
    int     m_cutType = 0;

};

#endif // CGRAPHICSCUTITEM_H
