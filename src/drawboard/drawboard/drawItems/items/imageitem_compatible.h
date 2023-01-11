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
#ifndef CPICTUREITEM_H
#define CPICTUREITEM_H

#include <QGraphicsPixmapItem>
#include <QPainterPath>

#include "rectitem.h"

class ImageItem_Compatible : public RectBaseItem
{
public:

    explicit ImageItem_Compatible(const QPixmap &pixmap = QPixmap(),
                                  PageItem *parent = nullptr,
                                  const QByteArray &fileSrcData = QByteArray());

    explicit ImageItem_Compatible(const QRectF &rect,
                                  const QPixmap &pixmap,
                                  PageItem *parent = nullptr,
                                  const QByteArray &fileSrcData = QByteArray());
    ~ImageItem_Compatible() override;

    SAttrisList attributions() override;
    void  setAttributionVar(int attri, const QVariant &var, int phase) override;

    /**
     * @brief type 图元的类型(图片)
     */
    int  type() const override;

    /**
     * @brief 加载图元的信息
     */
    void loadUnit(const Unit &data) override;

    /**
     * @brief 获取图元的信息
     */
    Unit getUnit(int reson) const override;

    /**
     * @brief 设置图片
     */
    void setPixmap(const QPixmap &pixmap);

    /**
     * @brief pixmap
     */
    QPixmap &pixmap();

    /**
     * @brief 设置旋转角度
     */
    void setAngle(const qreal &angle);

    /**
     * @brief isPosPenetrable 某一位置在图元上是否是可穿透的（透明的）
     * @param posLocal 该图元坐标系的坐标位置
     */
    bool isPosPenetrable(const QPointF &posLocal) override;

    /**
     * @brief isPosPenetrable 某一矩形区域在图元上是否是可穿透的（透明的）
     * @param rectLocal 该图元坐标系的某一矩形区域
     */
    bool isRectPenetrable(const QRectF &rectLocal) override;

    /**
     * @brief getSelfOrgShape 获取到自身的原始形状
     */
    QPainterPath calOrgShapeBaseRect(const QRectF &rect) const override;

    /**
     * @brief move  操作结束
     */
    void operatingEnd(PageItemEvent *event) override;

    /**
     * @brief move  操作结束
     */
    void operatingBegin(PageItemEvent *event) override;

    /**
      * @brief updateShape  刷新图元的形状(属性变化时调用重新计算图元的形状和样式)
      */
    //void updateShape() override;

protected:

    /**
     * @brief paintCache 绘制自身的样貌
     */
    void paintSelf(QPainter *painter, const QStyleOptionGraphicsItem *option) override;

protected:
    QPixmap m_pixmap;
    qreal   m_angle;
    QByteArray _srcByteArry;

    QTransform _trans;
};
#endif // CPICTUREITEM_H
