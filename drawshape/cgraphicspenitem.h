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
    virtual int  type() const Q_DECL_OVERRIDE;
    QPainterPath shape() const Q_DECL_OVERRIDE;
    QRectF boundingRect() const Q_DECL_OVERRIDE;
    virtual QRectF rect() const Q_DECL_OVERRIDE;
    virtual void duplicate(CGraphicsItem *item) Q_DECL_OVERRIDE;
    virtual CGraphicsUnit getGraphicsUnit() const Q_DECL_OVERRIDE;

    virtual void resizeTo(CSizeHandleRect::EDirection dir, const QPointF &point ) Q_DECL_OVERRIDE;
    virtual void resizeTo(CSizeHandleRect::EDirection dir, const QPointF &point,
                          bool bShiftPress, bool bAltPress ) Q_DECL_OVERRIDE;
    /**
     * @brief resizeTo 缩放矩形时，用于设置矩形大小与位置
     * @param dir 8个方向
     * @param offset x，y方向移动距离
     * @param xScale X轴放大缩小比例
     * @param yScale y轴放大缩小比例
     */
    virtual void resizeToMul(CSizeHandleRect::EDirection dir, const QPointF &offset,
                             const double &xScale, const double &yScale,
                             bool bShiftPress, bool bAltPress)override;

    virtual void resizeToMul_7(CSizeHandleRect::EDirection dir, QRectF pressRect,
                               QRectF itemPressRect, const qreal &xScale, const qreal &yScale,
                               bool bShiftPress, bool bAltPress) override Q_DECL_DEPRECATED;

    void updatePenPath(const QPointF &endPoint, bool isShiftPress);
    void updateCoordinate();
    void drawComplete();

    void setPath(const QPainterPath &path);
    QPainterPath getPath() const;

    void setPenStartpath(const QPainterPath &path);
    QPainterPath getPenStartpath() const;

    void setPenEndpath(const QPainterPath &path);
    QPainterPath getPenEndpath() const;

    void updatePenType(const ELineType &startType, const ELineType &endType);
    void setPixmap();

    void setDrawFlag(bool flag);
    void savePathBeforResize(QPainterPath path);

    void calcVertexes();
    /**
     * @brief getHighLightPath 获取高亮path
     * @return
     */
    virtual QPainterPath getHighLightPath() override;

    ELineType getPenStartType() const;
    void setPenStartType(const ELineType &penType);

    ELineType getPenEndType() const;
    void setPenEndType(const ELineType &penType);

protected:
    virtual void updateGeometry() Q_DECL_OVERRIDE;
    virtual void updateShape() Q_DECL_OVERRIDE {calcVertexes();}
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) Q_DECL_OVERRIDE;

private:
    QPainterPath m_path;
    QPainterPath m_pathBeforResize;//resize前保存路径信息
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
    void initHandle() override;
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
