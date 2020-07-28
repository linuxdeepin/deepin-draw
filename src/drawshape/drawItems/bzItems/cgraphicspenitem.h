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
class QPainterPath;
class CGraphicsPenItem : public CGraphicsItem
{
public:
    explicit CGraphicsPenItem(QGraphicsItem *parent = nullptr);
    explicit CGraphicsPenItem(const QPointF &startPoint, QGraphicsItem *parent = nullptr);
    explicit CGraphicsPenItem(const SGraphicsPenUnitData *data, const SGraphicsUnitHead &head, CGraphicsItem *parent = nullptr);
    virtual ~CGraphicsPenItem() Q_DECL_OVERRIDE;

    /**
     * @brief rect 重写实现画笔的矩形
     * @return
     */
    QRectF rect() const Q_DECL_OVERRIDE;

    /**
     * @brief rect 重写实现画笔的类型
     * @return
     */
    int type() const Q_DECL_OVERRIDE;

    /**
     * @brief getGraphicsUnit 获取图元的信息
     * @return
     */
    CGraphicsUnit getGraphicsUnit() const Q_DECL_OVERRIDE;

    /**
     * @brief resizeTo 重写实现画笔的resize逻辑
     * @return
     */
    void resizeTo(CSizeHandleRect::EDirection dir, const QPointF &point) Q_DECL_OVERRIDE;

    /**
     * @brief resizeTo 重写实现画笔的resize逻辑
     * @return
     */
    void resizeTo(CSizeHandleRect::EDirection dir, const QPointF &point,
                  bool bShiftPress, bool bAltPress) Q_DECL_OVERRIDE;

    /**
     * @brief resizeTo 缩放矩形时，用于设置矩形大小与位置
     * @param dir 8个方向
     * @param offset x，y方向移动距离
     * @param xScale X轴放大缩小比例
     * @param yScale y轴放大缩小比例
     */
    void resizeToMul(CSizeHandleRect::EDirection dir, const QPointF &offset,
                     const double &xScale, const double &yScale,
                     bool bShiftPress, bool bAltPress) override;

    /**
     * @brief updatePenPath 刷新画笔的路径信息（使用场景：toolCreatItemUpdate 创建该item时 刷新路径）
     * @return
     */
    void updatePenPath(const QPointF &endPoint, bool isShiftPress);

    /**
     * @brief updateCoordinate 刷新坐标系
     * @return
     */
    void updateCoordinate();

    /**
     * @brief drawComplete toolCreatItemFinished 绘制完成时调用
     * @return
     */
    void drawComplete();

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
     * @brief updatePenType 刷新设置起终点样式
     * @return
     */
    void updatePenType(const ELineType &startType, const ELineType &endType);

    /**
     * @brief setPixmap 得到一张场景的渲染图
     * @return
     */
    void setPixmap();

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
     * @brief loadGraphicsUnit 加载图元数据
     * @return
     */
    void loadGraphicsUnit(const CGraphicsUnit &data) Q_DECL_OVERRIDE;

protected:
    /**
     * @brief inSideShape 重写实现画笔的图元内部形状（rect类图元不包括边线）
     */
    QPainterPath inSideShape() const Q_DECL_OVERRIDE;

    /**
     * @brief duplicateCreatItem 创建一个同类型图元（未同步数据）
     */
    CGraphicsItem *duplicateCreatItem() Q_DECL_OVERRIDE;

    /**
     * @brief duplicate 同步数据信息到item
     */
    void duplicate(CGraphicsItem *item) Q_DECL_OVERRIDE;

    /**
     * @brief updateHandlesGeometry 刷新孩子节点
     */
    void updateHandlesGeometry() Q_DECL_OVERRIDE;

    /**
     * @brief updateShape 刷新图元形状
     */
    void updateShape() Q_DECL_OVERRIDE { calcVertexes(); }

    /**
     * @brief paint 绘制图元
     */
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget) Q_DECL_OVERRIDE;

private:
    QPainterPath m_path;
    QLineF m_straightLine;
    bool m_isShiftPress;
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

    void drawStart();
    void drawEnd();
};

#endif // CGRAPHICSPENITEM_H
