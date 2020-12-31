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
#include <QPainterPath>

class CGraphicsPolygonalStarItem : public CGraphicsRectItem
{
public:
    explicit CGraphicsPolygonalStarItem(int anchorNum = 0, int innerRadius = 0, CGraphicsItem *parent = nullptr);
    explicit CGraphicsPolygonalStarItem(int anchorNum, int innerRadius, const QRectF &rect, CGraphicsItem *parent = nullptr);
    explicit CGraphicsPolygonalStarItem(int anchorNum, int innerRadius, qreal x, qreal y, qreal w, qreal h,
                                        CGraphicsItem *parent = nullptr);

    /**
     * @brief type  图元类型
     * @return
     */
    int type() const override;

    /**
     * @brief setRect 设置图元在自身坐标系的包围矩形
     * @return
     */
    void setRect(const QRectF &rect) override;

    /**
     * @brief updatePolygonalStar 刷新设置锚点数和内元半径
     * @return
     */
    void updatePolygonalStar(int anchorNum, int innerRadius);

    /**
     * @brief setAnchorNum 设置锚点数
     * @return
     */
    void setAnchorNum(int num, bool preview = false);

    /**
     * @brief anchorNum 锚点数(多少个角的星星)
     * @return
     */
    int anchorNum() const;

    /**
     * @brief 内圆半径 （内角到中心的距离）
     * @return
     */
    void setInnerRadius(int radius, bool preview = false);

    /**
     * @brief 内圆半径 （内角到中心的距离）
     * @return
     */
    int innerRadius() const;

//    /**
//     * @brief 设置多边形路径
//     * @return
//     */
//    void setPolygon(const QPolygonF &polygon);

    /**
     * @brief loadGraphicsUnit 加载图元数据
     * @return
     */
    void loadGraphicsUnit(const CGraphicsUnit &data) override;

    /**
     * @brief loadGraphicsUnit 图元的数据
     * @return
     */
    CGraphicsUnit getGraphicsUnit(EDataReason reson) const override;

    /**
     * @brief updateShape 刷新图元形状
     * @return
     */
    void updateShape() override;

protected:
    /**
      * @brief selfOrgShape 图元的原始形状（rect类图元不包括边线）
      */
    QPainterPath getSelfOrgShape() const override;


    /**
     * @brief penStrokerShape 图元线条的形状（边线轮廓所组成的形状）
     */
    QPainterPath getTrulyShape() const override;


    /**
     * @brief shape 返回图元的外形状
     */
    QPainterPath getPenStrokerShape() const override;

    /**
     * @brief shape 返回真实显示的图元的外形状()
     */
    QPainterPath getShape() const override;

    /**
     * @brief paint 绘制图元
     * @return
     */
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

private:
    int m_anchorNum[2] = {5, 5}; //描点数
    int m_innerRadius[2] = {50, 50}; //内接圆半径
    bool m_preview[2] = {false, false};

    //区分描边的绘制方式；1.RenderPathLine 把描边的边描述成一个包裹的路径 通过brush方式填充它的颜色
    //                 2.PaintPolyLine  以Qt自带的绘制多边形方式绘制边线(不设置填充色只设置QPen颜色)
    enum ERenderWay {RenderPathLine, PaintPolyLine};

    //const ERenderWay m_renderWay = RenderPathLine;
    QPolygonF  m_polygonForBrush;
    QPolygonF  m_polygonPen;
    QPolygonF  m_hightlightPath;

    QPainterPath m_pathForRenderPenLine;

private:
    void calcPolygon();
    void calcPolygon_helper(QPolygonF &outPolygon, int n, qreal offset = 0.0);
};

#endif // CGRAPHICSPOLYGONALSTARITEM_H
