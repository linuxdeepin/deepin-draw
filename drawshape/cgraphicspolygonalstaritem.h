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
#ifndef CGRAPHICSPOLYGONALSTARITEM_H
#define CGRAPHICSPOLYGONALSTARITEM_H


#include "cgraphicsrectitem.h"

class CGraphicsPolygonalStarItem : public CGraphicsRectItem
{
public:
    explicit CGraphicsPolygonalStarItem(int anchorNum = 0, int innerRadius = 0, CGraphicsItem *parent = nullptr);
    explicit CGraphicsPolygonalStarItem(int anchorNum, int innerRadius, const QRectF &rect, CGraphicsItem *parent = nullptr);
    explicit CGraphicsPolygonalStarItem(int anchorNum, int innerRadius, qreal x, qreal y, qreal w, qreal h, CGraphicsItem *parent = nullptr);
    explicit CGraphicsPolygonalStarItem(const SGraphicsPolygonStarUnitData *data, const SGraphicsUnitHead &head, CGraphicsItem *parent = nullptr);
    virtual QPainterPath shape() const Q_DECL_OVERRIDE;
    virtual QRectF boundingRect() const Q_DECL_OVERRIDE;
    virtual  int type() const Q_DECL_OVERRIDE;
    virtual void resizeTo(CSizeHandleRect::EDirection dir,
                          const QPointF &point,
                          bool bShiftPress, bool bAltPress) Q_DECL_OVERRIDE;
    /**
     * @brief duplicate 拷贝自己
     * @return
     */
    virtual void duplicate(CGraphicsItem *item) Q_DECL_OVERRIDE;
    void setRect(const QRectF &rect) Q_DECL_OVERRIDE;
    void updatePolygonalStar(int anchorNum, int innerRadius);
    int anchorNum() const;
    int innerRadius() const;

    void setPolygon(const QPolygonF &polygon);


    virtual CGraphicsUnit getGraphicsUnit() const Q_DECL_OVERRIDE;
    /**
     * @brief getHighLightPath 获取高亮path
     * @return
     */
    virtual QPainterPath getHighLightPath() Q_DECL_OVERRIDE;

protected:
    virtual void updateShape() Q_DECL_OVERRIDE {calcPolygon();}
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) Q_DECL_OVERRIDE;

private:
    int m_anchorNum;    //描点数
    int m_innerRadius;  //内接圆半径

    //区分描边的绘制方式；1.RenderPathLine 把描边的边描述成一个包裹的路径 通过brush方式填充它的颜色
    //                 2.PaintPolyLine  以Qt自带的绘制多边形方式绘制边线(不设置填充色只设置QPen颜色)
    enum ERenderWay {RenderPathLine, PaintPolyLine};

    ERenderWay m_renderWay = PaintPolyLine;
    QPolygonF  m_polygonForBrush;
    QPolygonF  m_polygonPen;

    QPainterPath m_pathForRenderPenLine;

private:
    void calcPolygon();
    void calcPolygon_helper(QPolygonF &outPolygon, int n, qreal offset = 0.0);
};

#endif // CGRAPHICSPOLYGONALSTARITEM_H
