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
    explicit CGraphicsRectItem(const SGraphicsRectUnitData &rectData, const SGraphicsUnitHead &head, CGraphicsItem *parent = nullptr);
    virtual ~CGraphicsRectItem()  override;

    /**
     * @brief type 图元的类型
     * @return
     */
    int type() const override;

    /**
     * @brief setRect 创建矩形时，用于设置矩形大小
     * @param rect
     */
    virtual void setRect(const QRectF &rect);

    /**
     * @brief rect 矩形的大小
     * @param rect
     */
    virtual QRectF rect() const override;

    virtual void resizeTo(CSizeHandleRect::EDirection dir, const QPointF &point) override;

    virtual void resizeTo(CSizeHandleRect::EDirection dir, const QPointF &point,
                          bool bShiftPress, bool bAltPress) override;
    /**
     * @brief resizeTo 缩放矩形时，用于设置矩形大小与位置
     * @param dir 8个方向
     * @param offset x，y方向移动距离
     * @param xScale X轴放大缩小比例
     * @param yScale y轴放大缩小比例
     */
    virtual void resizeToMul(CSizeHandleRect::EDirection dir, const QPointF &offset,
                             const double &xScale, const double &yScale,
                             bool bShiftPress, bool bAltPress) override;

    /**
     * @brief loadGraphicsUnit 加载图元数据
     * @return
     */
    void loadGraphicsUnit(const CGraphicsUnit &data, bool allInfo) override;

    /**
     * @brief getGraphicsUnit 得到图元的数据
     * @return
     */
    CGraphicsUnit getGraphicsUnit(bool all) const override;

    /**
     * @brief setXYRedius 设置矩形圆角半径
     * @param xRedius x方向半径
     * @param yRedius y方向半径
     */
    void setXYRedius(int xRedius, int yRedius, bool preview = false);

    /**
     * @brief getXYRedius 默认返回一个，返回圆角半径
     * @return
     */
    int getXRedius();

    /**
     * @brief getXYRedius 默认返回一个，返回圆角半径
     * @return
     */
    int getPaintRedius();

protected:
    void loadGraphicsRectUnit(const SGraphicsRectUnitData &rectData);

protected:
    /**
     * @brief inSideShape 图元内部形状（rect类图元不包括边线）
     */
    QPainterPath inSideShape() const override;

    /**
     * @brief incLength 虚拟的额外线宽宽度（解决选中困难的问题 提升用户体验,但闭合形状很容易选中，所以返回0）
     * @return 返回额外的线宽（0）
     */
    qreal incLength() const override;

    /**
     * @brief duplicateCreatItem 复制一个一样类型的图元
     * @return
     */
    CGraphicsItem *duplicateCreatItem() override;

    /**
     * @brief duplicate 同步数据到item
     * @return
     */
    void duplicate(CGraphicsItem *item) override;

    /**
     * @brief paint 绘制图元
     * @return
     */
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget = nullptr) override;

private:
    QPointF m_topLeftPoint; //左上角的点
    QPointF m_bottomRightPoint; //右下角的点
    int m_xRedius;
    int m_yRedius;

    int m_rediusForPreview = 5;
    bool m_isPreviewRedius = false;
};

#endif // CGRAPHICSRECTITEM_H
