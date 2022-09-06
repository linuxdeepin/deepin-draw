// SPDX-FileCopyrightText: 2020 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef CGRAPHICSTRIANGLEITEM_H
#define CGRAPHICSTRIANGLEITEM_H

#include "cgraphicsrectitem.h"
#include <QPainterPath>

class CGraphicsTriangleItem : public CGraphicsRectItem
{
public:
    explicit CGraphicsTriangleItem(CGraphicsItem *parent = nullptr);
    explicit CGraphicsTriangleItem(const QRectF &rect, CGraphicsItem *parent = nullptr);
    explicit CGraphicsTriangleItem(qreal x, qreal y, qreal w, qreal h, CGraphicsItem *parent = nullptr);

    DrawAttribution::SAttrisList attributions() override;

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

protected:
    /**
      * @brief selfOrgShape 图元的原始形状（rect类图元不包括边线）
      */
    QPainterPath getSelfOrgShape() const override;

    /**
     * @brief paint 绘制图元
     * @return
     */
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget) override;

private:
    void calcPoints_helper(QVector<QPointF> &outVector, qreal offset = 0.0) const;


    QPolygonF polyForPen;
};

#endif // CGRAPHICSTRIANGLEITEM_H
