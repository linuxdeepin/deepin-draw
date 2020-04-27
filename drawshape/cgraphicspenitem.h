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

class CGraphicsPenItem : public CGraphicsItem
{
public:
    static CGraphicsPenItem *s_curPenItem;
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
    virtual void resizeTo(CSizeHandleRect::EDirection dir, const QPointF &point, bool bShiftPress, bool bAltPress ) Q_DECL_OVERRIDE;
    void updatePenPath(const QPointF &endPoint, bool isShiftPress);
    EPenType currentType() const;
    void setCurrentType(const EPenType &currentType);
    void updateCoordinate();
    void drawComplete();

    void setPath(const QPainterPath &path);

    void setArrow(const QPolygonF &arrow);
    QPolygonF getArrow() const;
    QPainterPath getPath() const;

    void updatePenType(const EPenType &currentType);
    void setPixmap();

    void setDrawFlag(bool flag);

    void calcVertexes();

protected:
    virtual void updateGeometry() Q_DECL_OVERRIDE;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) Q_DECL_OVERRIDE;

private:
    QPainterPath m_path;
    QList<QPointF> m_points;
    QPointF        m_recordPrePos;

    QPolygonF m_arrow; //箭头三角形
    QLineF m_straightLine;
    bool m_isShiftPress;
    QVector<QPointF> m_smoothVector;
    EPenType m_currentType;
    QPixmap m_tmpPix;
    bool m_isDrawing;//是否正在绘图
    int m_drawIndex;
    QPointF m_point4;

private:
    void initPen();
    /**
     * @brief calcVertexes 计算箭头三角形的三个点
     * @param prePoint 前一个点
     * @param currentPoint 当前点
     */
    void calcVertexes(const QPointF &prePoint, const QPointF &currentPoint);



    //qreal GetThreeBezierValue(qreal p0, qreal p1, qreal p2, qreal p3, qreal t);

    //QPointF GetThreeBezierValue(QPainterPath::Element p0, QPainterPath::Element p1, QPainterPath::Element p2, QPainterPath::Element p3, qreal t);
    qreal GetBezierValue(qreal p0, qreal p1, qreal p2, qreal p3, qreal p4, qreal p5, qreal t);
    QPointF GetBezierValue(QPainterPath::Element p0, QPainterPath::Element p1, QPainterPath::Element p2, QPainterPath::Element p3, QPainterPath::Element p4, QPainterPath::Element p5, qreal t);
};

#endif // CGRAPHICSPENITEM_H
