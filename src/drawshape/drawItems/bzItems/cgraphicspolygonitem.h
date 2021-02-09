/*
 * Copyright (C) 2019 ~ %YEAR% Deepin Technology Co., Ltd.
 *
 * Author:     WangXin
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
#ifndef CGRAPHICSPOLYGONITEM_H
#define CGRAPHICSPOLYGONITEM_H

#include "csizehandlerect.h"
#include "cgraphicsrectitem.h"
#include <QPainterPath>
#include <QtGlobal>
#include <QVector>
#include <QRectF>

class CGraphicsPolygonItem : public CGraphicsRectItem
{
public:
    explicit CGraphicsPolygonItem(int count = 0, CGraphicsItem *parent = nullptr);
    explicit CGraphicsPolygonItem(int count, const QRectF &rect, CGraphicsItem *parent = nullptr);
    explicit CGraphicsPolygonItem(int count, qreal x, qreal y, qreal w, qreal h, CGraphicsItem *parent = nullptr);

    /**
     * @brief type 图元的类型
     * @return
     */
    int type() const override;

    /**
     * @brief loadGraphicsUnit 加载图元数据
     * @return
     */
    void loadGraphicsUnit(const CGraphicsUnit &data) override;

    /**
     * @brief getGraphicsUnit 得到图元数据
     * @return
     */
    CGraphicsUnit getGraphicsUnit(EDataReason reson) const override;

    /**
     * @brief setPointCount 设置侧边数（根据当前rect大小进行自动计算出一个多边形）
     * @return
     */
    void setPointCount(int num, bool preview = false);

    /**
     * @brief setPointCount 侧边数
     * @return
     */
    int nPointsCount() const;

protected:
    /**
      * @brief selfOrgShape 图元的原始形状（rect类图元不包括边线）
      */
    QPainterPath getSelfOrgShape() const override;

private:
    static void calcPoints_helper(QVector<QPointF> &outVector, int n, const QRectF &rect, qreal offset = 0.0);

private:
    int m_nPointsCount[2] = {5, 5};     //点数
    bool m_isPreviewPointCount = false; //点数是否是预览值

    friend class CGraphicsPolygonalStarItem;
};

#endif // CGRAPHICSPOLYGONITEM_H
