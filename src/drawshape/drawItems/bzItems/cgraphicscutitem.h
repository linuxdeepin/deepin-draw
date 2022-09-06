// SPDX-FileCopyrightText: 2020 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef CGRAPHICSCUTITEM_H
#define CGRAPHICSCUTITEM_H

#include "cgraphicsitem.h"
#include <QPainterPath>

class CSizeHandleRect;
class QGraphicsProxyWidget;

class CGraphicsCutItem : public CGraphicsItem
{

public:
    explicit CGraphicsCutItem(CGraphicsItem *parent = nullptr);
    explicit CGraphicsCutItem(const QRectF &rect, CGraphicsItem *parent = nullptr);
    ~CGraphicsCutItem()  override;

    DrawAttribution::SAttrisList attributions() override;

    /**
     * @brief type 图元的类型
     */
    int  type() const override;

    /**
     * @brief type 图元的包裹矩形
     */
    QRectF boundingRect() const override;

    /**
     * @brief shape 返回图元的形状
     */
    QPainterPath shape() const override;

    /**
     * @brief contains 点是否在图元中（重载实现更好选中，增加用户体验）
     * @param point在图元本地坐标系的坐标值
     */
    bool contains(const QPointF &point) const override;

    /**
     * @brief setRect 设置裁剪矩形
     */
    void setRect(const QRectF &rect);

    /**
     * @brief setSize 设置裁剪矩形的大小
     */
    void setSize(int w, int h);


    void setOriginalInitRect(const QRectF &size);

    void setOriginalRect(const QRectF &size);
    QRectF originalRect() const;

    /**
     * @brief rect 裁剪矩形
     */
    QRectF rect() const override;

    /**
     * @brief setRatioType 设置裁剪比例的类型
     */
    void     setRatioType(ECutType type);

    /**
     * @brief getRatioType 裁剪比例的类型
     */
    ECutType getRatioType() const;


    /**
     * @brief isFreeMode 当前裁剪类型是否是自由裁剪模式
     */
    bool isFreeMode() const;

    /**
     * @brief setIsFreeMode 设置裁剪类型是否是自由裁剪模式
     */
    void setIsFreeMode(bool isFreeMode);


    /**
     * @brief getWHRadio 获取当前的裁剪比例
     */
    qreal getWHRadio();

    /**
     * @brief resizeTo 调整裁剪矩形大小
     */
    void  resizeCutSize(CSizeHandleRect::EDirection dir, const QPointF &prePoint, const QPointF &point,
                        QPointF *outAcceptPos = nullptr);

    /**
     * @brief move 调整裁剪矩形位置
     */
    void   move(QPointF beginPoint, QPointF movePoint) override;

    /**
     * @brief getHighLightPath 获取高亮path
     * @return
     */
    QPainterPath getHighLightPath() override;

    /**
     * @brief isPosPenetrable 某一位置在图元上是否是可穿透的（透明的）(基于inSideShape和outSideShape)
     * @param posLocal 该图元坐标系的坐标位置
     */
    bool isPosPenetrable(const QPointF &posLocal) override;

    /**
     * @brief nodes  获取操作缩放节点
     */
    Handles nodes();

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;

    QVariant itemChange(GraphicsItemChange doChange, const QVariant &value) override;

    void updateHandlesGeometry() override;

private:
    /**
     * @brief initRect 初始化矩形的属性和边框小方块
     */
    void initHandle() override;
    void drawFourConner(QPainter *painter);
    void drawTrisectorRect(QPainter *painter);
    void showControlRects(bool);

private:
    QPointF  m_topLeftPoint;      //左上角的点
    QPointF  m_bottomRightPoint;  //右下角的点
    QRectF   m_originalInitRect;
    QRectF   m_originalRect;
    bool     m_isFreeMode;
    ECutType m_cutType = cut_free;

};

#endif // CGRAPHICSCUTITEM_H
