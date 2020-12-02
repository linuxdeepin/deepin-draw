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
#ifndef CGRAPHICSPENITEM_H
#define CGRAPHICSPENITEM_H

#include "cgraphicsitem.h"
#include "csizehandlerect.h"

#include <QGraphicsItem>
#include <QPainterPath>
class QPainterPath;
class CGraphicsPenItem : public CGraphicsItem
{
public:
    explicit CGraphicsPenItem(QGraphicsItem *parent = nullptr);
    explicit CGraphicsPenItem(const QPointF &startPoint, QGraphicsItem *parent = nullptr);
    ~CGraphicsPenItem() override;

    /**
     * @brief rect 重写实现画笔的矩形
     * @return
     */
    QRectF rect() const override;

    /**
     * @brief rect 重写实现画笔的类型
     * @return
     */
    int type() const override;

    /**
     * @brief doChangeSelf 实现图元变化
     * @param event    描述了应该如何变化的抽象
     */
    void doChangeSelf(CGraphItemEvent *event) override;

    /**
     * @brief getGraphicsUnit 获取图元的信息
     * @return
     */
    CGraphicsUnit getGraphicsUnit(EDataReason reson) const override;

    /**
     * @brief updatePenPath 刷新画笔的路径信息（使用场景：toolCreatItemUpdate 创建该item时 刷新路径）
     * @return
     */
    void updatePenPath(const QPointF &endPoint, bool isShiftPress);

    /**
     * @brief drawComplete toolCreatItemFinished 绘制完成时调用
     * @return
     */
    void drawComplete(bool doBz = false);

    /**
     * @brief setPath 设置路径
     * @return
     */
    void setPath(const QPainterPath &path);

    /**
     * @brief getPath 获取路径
     * @return
     */
    QPainterPath getPath() const;

    /**
     * @brief setPenStartpath 设置起点样式
     * @return
     */
    void setPenStartpath(const QPainterPath &path);

    /**
     * @brief getPenStartpath 返回起点样式
     * @return
     */
    QPainterPath getPenStartpath() const;

    /**
     * @brief setPenEndpath 设置终点样式
     * @return
     */
    void setPenEndpath(const QPainterPath &path);

    /**
     * @brief getPenEndpath 获取终点样式
     * @return
     */
    QPainterPath getPenEndpath() const;

    /**
     * @brief setDrawFlag true表示正在绘制，false表示没有
     * @return
     */
    void setDrawFlag(bool flag);

    /**
     * @brief calcVertexes 根据路径计算绘制路径
     * @return
     */
    void calcVertexes();

    /**
     * @brief getHighLightPath 获取高亮path
     * @return
     */
    virtual QPainterPath getHighLightPath() override;

    /**
     * @brief getPenStartType 得到起点的样式
     * @return
     */
    ELineType getPenStartType() const;

    /**
     * @brief setPenStartType 设置起点的样式
     * @return
     */
    void setPenStartType(const ELineType &penType);

    /**
     * @brief getPenEndType 得到终点的样式
     * @return
     */
    ELineType getPenEndType() const;

    /**
     * @brief setPenEndType 设置终点的样式
     * @return
     */
    void setPenEndType(const ELineType &penType);

    /**
     * @brief straightLine 得到绘制过程中的可能存在的直线线条
     * @return
     */
    QLineF straightLine();

    /**
     * @brief loadGraphicsUnit 加载图元数据
     * @return
     */
    void loadGraphicsUnit(const CGraphicsUnit &data) override;

protected:
    /**
     * @brief inSideShape 重写实现画笔的图元原始形状（图元不包括边线）
     */
    QPainterPath getSelfOrgShape() const override;

    /**
     * @brief penStrokerShape 图元线条的形状（边线轮廓所组成的形状）
     */
    QPainterPath getPenStrokerShape() const override;

    /**
     * @brief updateHandlesGeometry 刷新孩子节点
     */
    void updateHandlesGeometry() override;

    /**
     * @brief updateShape 刷新图元形状
     */
    void updateShape() override;

    /**
     * @brief paint 绘制图元
     */
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget) override;

    /**
     * @brief paintCache 绘制换冲图(如果启动了缓冲图，记得重载该函数描述位图的样貌)
     */
    void paintSelf(QPainter *painter, const QStyleOptionGraphicsItem *option) override;

    /**
     * @brief move  操作开始
     */
    void operatingBegin(CGraphItemEvent *event) override;

    /**
     * @brief move  操作结束
     */
    void operatingEnd(CGraphItemEvent *event) override;

    /**
     * @brief isPosPenetrable  是否可以进行穿透
     */
    virtual bool isPosPenetrable(const QPointF &posLocal) override;

protected:
    QLineF m_straightLine;
    bool m_isShiftPress;

private:
    QPainterPath m_path;
    QVector<QPointF> m_smoothVector;
    QPixmap m_tmpPix;
    bool m_isDrawing;//是否正在绘图
    int m_drawIndex;
    QPointF m_point4;

    // 画笔类型
    ELineType m_penStartType; // 画笔起点样式
    ELineType m_penEndType; // 画笔终点样式
    QPainterPath m_startPath;
    QPainterPath m_endPath;
    bool m_isStartWithLine = false; // 用于判断是否是开始以画直线为起点
    bool m_isEndWithLine = false; // 用于判断是否是结束以画直线为起点

private:
    /**
     * @brief calcVertexes 计算箭头三角形的三个点
     * @param prePoint 前一个点
     * @param currentPoint 当前点
     */
    void calcVertexes(const QPointF &prePoint, const QPointF &currentPoint);

    qreal GetBezierValue(qreal p0, qreal p1, qreal p2, qreal p3, qreal p4, qreal p5, qreal t);
    QPointF GetBezierValue(QPainterPath::Element p0, QPainterPath::Element p1, QPainterPath::Element p2, QPainterPath::Element p3, QPainterPath::Element p4, QPainterPath::Element p5, qreal t);

    void updateStartPathStyle();
    void updateEndPathStyle();
};

#endif // CGRAPHICSPENITEM_H
