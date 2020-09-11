/*
 * Copyright (C) 2019 ~ %YEAR% Deepin Technology Co., Ltd.
 *
 * Author:     JiangChangli
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
#include "cgraphicsrectitem.h"

class CPictureItem : public  CGraphicsRectItem
{
public:
    explicit CPictureItem(const QPixmap &pixmap = QPixmap(), CGraphicsItem *parent = nullptr, const QByteArray &fileSrcData = QByteArray());
    explicit CPictureItem(const QRectF &rect, const QPixmap &pixmap, CGraphicsItem *parent = nullptr, const QByteArray &fileSrcData = QByteArray());
    explicit CPictureItem(const SGraphicsPictureUnitData *data, const SGraphicsUnitHead &head, CGraphicsItem *parent = nullptr);
    ~CPictureItem() Q_DECL_OVERRIDE;
    int  type() const Q_DECL_OVERRIDE;

    void setRotation90(bool leftOrRight);
//    bool getAdjustScence();

    enum EFilpDirect {EFilpHor, EFilpVer};

    /**
     * @brief doFilp 在当前基础上翻转一下
     * @return
     */
    void doFilp(EFilpDirect dir = EFilpHor);

    /**
     * @brief setFilpBaseOrg 设置在初始图像上是否翻转,否则和原图一致
     */
    void setFilpBaseOrg(EFilpDirect dir, bool b);

    /**
     * @brief isFilped 图像是否翻转过(相对原图)
     */
    bool isFilped(EFilpDirect dir);

    /**
     * @brief duplicate 拷贝自己
     * @return
     */
    virtual void duplicate(CGraphicsItem *item) Q_DECL_OVERRIDE;
    /**
     * @brief 创建一个同类型图元
     */
    virtual CGraphicsItem *duplicateCreatItem() Q_DECL_OVERRIDE;

    void loadGraphicsUnit(const CGraphicsUnit &data, bool allInfo) override;
    CGraphicsUnit getGraphicsUnit(bool all) const Q_DECL_OVERRIDE;
    QPainterPath getHighLightPath() Q_DECL_OVERRIDE;

    void setPixmap(const QPixmap &pixmap);

    void setAngle(const qreal &angle);

    /**
     * @brief isPosPenetrable 某一位置在图元上是否是可穿透的（透明的）(基于inSideShape和outSideShape)
     * @param posLocal 该图元坐标系的坐标位置
     */
    bool isPosPenetrable(const QPointF &posLocal) Q_DECL_OVERRIDE;

protected:
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) Q_DECL_OVERRIDE;

private:
    QPixmap m_pixmap;
    qreal m_angle;
    QByteArray _srcByteArry;
    bool m_adjustScence = false;

    bool flipHorizontal; // 水平翻转
    bool flipVertical;   // 垂直翻转
};

#endif // CPICTUREITEM_H
