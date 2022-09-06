// SPDX-FileCopyrightText: 2020 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef CPICTUREITEM_H
#define CPICTUREITEM_H

#include <QGraphicsPixmapItem>
#include <QPainterPath>

#include "cgraphicsrectitem.h"

class CPictureItem : public CGraphicsRectItem
{
public:

    explicit CPictureItem(const QPixmap &pixmap = QPixmap(),
                          CGraphicsItem *parent = nullptr,
                          const QByteArray &fileSrcData = QByteArray());

    explicit CPictureItem(const QRectF &rect,
                          const QPixmap &pixmap,
                          CGraphicsItem *parent = nullptr,
                          const QByteArray &fileSrcData = QByteArray());
    ~CPictureItem() override;

    DrawAttribution::SAttrisList attributions() override;
    void  setAttributionVar(int attri, const QVariant &var, int phase) override;

    /**
     * @brief type 图元的类型(图片)
     */
    int  type() const override;

    //void setRotation90(bool leftOrRight);

    /**
     * @brief 加载图元的信息
     */
    void loadGraphicsUnit(const CGraphicsUnit &data) override;

    /**
     * @brief 获取图元的信息
     */
    CGraphicsUnit getGraphicsUnit(EDataReason reson) const override;

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
    QPainterPath getSelfOrgShape() const override;

    /**
     * @brief move  操作结束
     */
    void operatingEnd(CGraphItemEvent *event) override;

    /**
     * @brief move  操作结束
     */
    void operatingBegin(CGraphItemEvent *event) override;

    /**
      * @brief updateShape  刷新图元的形状(属性变化时调用重新计算图元的形状和样式)
      */
    void updateShape() override;

    /**
      * @brief isBlurEnable  是否支持模糊
      */
    bool isBlurEnable()const override;
protected:

    /**
     * @brief paint 绘制图元
     * @return
     */
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget = nullptr) override;
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
