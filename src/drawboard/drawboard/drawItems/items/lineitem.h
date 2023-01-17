/*
 * Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
 *
 * Author:     Ji XiangLong <jixianglong@uniontech.com>
 *
 * Maintainer: WangYu <wangyu@uniontech.com>
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
#ifndef CGRAPHICSLINEITEM_H
#define CGRAPHICSLINEITEM_H
#include "vectoritem.h"
#include <QPainterPath>
#include <QLineF>

class DRAWLIB_EXPORT LineBaseItem: public VectorItem
{
public:
    LineBaseItem(PageItem *parent = nullptr);
    LineBaseItem(const QLineF &line, PageItem *parent = nullptr);

    QLineF line()const;
    void setLine(const QPointF &begin, const QPointF &end);
    void setLine(const QLineF &line);

    virtual QPainterPath calOrgShapeBaseLine(const QLineF &line) const = 0;
protected:
    QPainterPath getOrgShape() const final;
    /**
     * @brief penStrokerShape 图元线条的形状（边线轮廓所组成的形状）
     */
    QPainterPath getPenStrokerShape() const override;
private:
    /**
     * @brief incLength 虚拟的额外线宽宽度（解决选中困难的问题 提升用户体验）
     * @return 返回额外的线宽（一般与当前的放大值相关）
     */
    virtual qreal incLength() const;
protected:
    QLineF _line;
};

class DRAWLIB_EXPORT LineItem : public LineBaseItem
{
public:
    LineItem(PageItem *parent = nullptr);
    LineItem(const QLineF &line, PageItem *parent = nullptr);
    LineItem(const QPointF &p1, const QPointF &p2, PageItem *parent = nullptr);
    ~LineItem() override;

    SAttrisList attributions() override;
    void  setAttributionVar(int attri, const QVariant &var, int phase) override;

    int type() const override;

    void doScaling(PageItemScalEvent *event) override;
    bool testScaling(PageItemScalEvent *event) override;

    Unit getUnit(int reson) const override;
    void loadUnit(const Unit &data) override;

    void setPos1Style(ELinePosStyle style);
    ELinePosStyle pos1Style() const;

    void setPos2Style(ELinePosStyle style);
    ELinePosStyle pos2Style() const;

protected:
    QPainterPath calOrgShapeBaseLine(const QLineF &line) const override;
    void paintSelf(QPainter *painter, const QStyleOptionGraphicsItem *option) override;
    bool isPosPenetrable(const QPointF &posLocal) override;

    PRIVATECLASS(LineItem)
};

class DRAWLIB_EXPORT LineHandleNode: public HandleNode
{
    Q_OBJECT
public:
    enum ENodeTpe {LinePos1 = Resize_LT, LinePos2 = Resize_RB};
    LineHandleNode(ENodeTpe tp, LineItem *parent);
    LineItem *parentLineItem()const;
protected:
    void setNodePos() override;
};

#endif // CGRAPHICSLINEITEM_H
