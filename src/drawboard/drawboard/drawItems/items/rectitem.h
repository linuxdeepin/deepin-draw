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
#ifndef CGRAPHICSRECTITEM_H
#define CGRAPHICSRECTITEM_H
#include "csizehandlerect.h"
#include "vectoritem.h"
#include "pagecontext.h"
#include <QtGlobal>
#include <QPainterPath>
#include <QVector>
#include <QRectF>

class DRAWLIB_EXPORT RectBaseItem : public VectorItem
{
public:
    explicit RectBaseItem(PageItem *parent = nullptr);
    explicit RectBaseItem(const QRectF &rect, PageItem *parent = nullptr);
    explicit RectBaseItem(qreal x, qreal y, qreal w, qreal h, PageItem *parent = nullptr);
    ~RectBaseItem()  override;

    QRectF rect() const;
    void setRect(const QRectF &rect);

    void doScaling(PageItemScalEvent *event) override;
    bool testScaling(PageItemScalEvent *event) override;

    void loadUnit(const Unit &data) override;
    Unit getUnit(int reson) const override;

protected:
    virtual QPainterPath calOrgShapeBaseRect(const QRectF &rect) const = 0;

protected:
    QPainterPath getOrgShape() const final;

protected:
    void loadGraphicsRectUnit(const RectUnitData &rectData);

private:
    QRectF _rct;
};

class RectItem: public RectBaseItem
{
public:
    using RectBaseItem::RectBaseItem;

    int type() const override;

    SAttrisList attributions() override;
    void  setAttributionVar(int attri, const QVariant &var, int phase) override;

    void setXYRedius(int xRedius, int yRedius, bool preview = false);
    int  getXRedius()const;

protected:
    void loadUnit(const Unit &ut) override;
    Unit getUnit(int reson) const override;

    QPainterPath calOrgShapeBaseRect(const QRectF &rect) const override;
    void paintSelf(QPainter *painter, const QStyleOptionGraphicsItem *option) override;
private:
    int  m_xRedius = 5;
    int  m_yRedius = 5;
    int  m_rediusForPreview = 0;
    bool m_isPreviewRedius  = false;
};

#endif // CGRAPHICSRECTITEM_H
