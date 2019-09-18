/*
 * Copyright (C) 2019 ~ %YEAR% Deepin Technology Co., Ltd.
 *
 * Author:     WangXing
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
#include "cgraphicsitem.h"
#include "cdrawparamsigleton.h"
#include <QtGlobal>
#include <QVector>
#include <QRectF>
#include <QGraphicsBlurEffect>

class CSizeHandleRect;
class CGraphicsRectItem : public CGraphicsItem
{
public:
    explicit CGraphicsRectItem(CGraphicsItem *parent = nullptr);
    explicit CGraphicsRectItem(const QRectF &rect, CGraphicsItem *parent = nullptr);
    explicit CGraphicsRectItem(qreal x, qreal y, qreal w, qreal h, CGraphicsItem *parent = nullptr);
    explicit CGraphicsRectItem(const CGraphicsUnit &unit, CGraphicsItem *parent = nullptr);
    virtual ~CGraphicsRectItem()  Q_DECL_OVERRIDE;
    virtual int  type() const Q_DECL_OVERRIDE;
    /**
     * @brief setRect 创建矩形时，用于设置矩形大小
     * @param rect
     */
    virtual void setRect(const QRectF &rect);
    virtual QRectF rect() const Q_DECL_OVERRIDE;
    virtual QRectF boundingRect() const Q_DECL_OVERRIDE;
    virtual void resizeTo(CSizeHandleRect::EDirection dir, const QPointF &point ) Q_DECL_OVERRIDE;
    virtual void resizeTo(CSizeHandleRect::EDirection dir, const QPointF &point, bool bShiftPress, bool bAltPress ) Q_DECL_OVERRIDE;
    /**
     * @brief duplicate 拷贝自己
     * @return
     */
    CGraphicsItem *duplicate() const Q_DECL_OVERRIDE;
    virtual CGraphicsUnit getGraphicsUnit() const Q_DECL_OVERRIDE;

protected:
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) Q_DECL_OVERRIDE;
    virtual void updateGeometry() Q_DECL_OVERRIDE;

private:
    /**
     * @brief initRect 初始化矩形的属性和边框小方块
     */
    void initRect();

private:
    QPointF m_topLeftPoint; //左上角的点
    QPointF m_bottomRightPoint; //右下角的点
    ESelectionHandleState m_state; //边框选中状态
};

#endif // CGRAPHICSRECTITEM_H
