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
#ifndef CDRAWSCENE_H
#define CDRAWSCENE_H
#include "cselecttool.h"
#include <QGraphicsScene>
#include <QObject>

class QGraphicsSceneMouseEvent;
class QKeyEvent;
class CGraphicsPolygonItem;
class CGraphicsPolygonalStarItem;

class CDrawScene : public QGraphicsScene
{
    Q_OBJECT
    friend CSelectTool;

public:
    explicit CDrawScene(QObject *parent = nullptr);

    /**
     * @brief keyEvent 从绘图工具返回键盘事件
     * @param keyEvent
     */
    void keyEvent(QKeyEvent *keyEvent);
    /**
     * @brief setCursor 设置鼠标指针形状
     * @param cursor
     */
    void setCursor(const QCursor &cursor);

    /**
     * @brief attributeChanged 画笔画刷属性更改
     */
    void attributeChanged();

    /**
     * @brief changeAttribute 根据选中的图元修改当前画笔画刷属性
     */
    void changeAttribute(bool flag, QGraphicsItem *selectedItem);

    /**
     * @brief mouseEvent 鼠标事件
     * @param mouseEvent
     */
    void mouseEvent(QGraphicsSceneMouseEvent *mouseEvent);

    /**
     * @brief drawBackground
     * @param painter
     * @param rect
     */
    void drawBackground(QPainter *painter, const QRectF &rect) Q_DECL_OVERRIDE;
    /**
     * @brief showCutItem 显示裁剪图元
     */
    void showCutItem();
    /**
     * @brief quitCutMode 退出裁剪
     */
    void quitCutMode();

    void setItemDisable(bool canSelecte);

signals:
    /**
     * @brief signalAttributeChanged 发送属性栏更改的信号
     * @param flag
     * @param primitiveType 图元类型
     */
    void signalAttributeChanged(bool flag, int primitiveType);
    /**
     * @brief signalChangeToSelect 发送工具栏切换为选择的信号
     */
    void signalChangeToSelect();
    /**
     * @brief signalQuitCutMode 退出裁剪模式
     */
    void signalQuitCutMode();

    /**
     * @brief itemMoved 移动
     * @param item
     * @param pos
     */
    void itemMoved(QGraphicsItem *item, QPointF pos);

    void itemAdded(QGraphicsItem *item);

    void itemRotate(QGraphicsItem *item, const qreal oldAngle );

    void itemResize(CGraphicsItem *item, CSizeHandleRect::EDirection handle, QPointF beginPos, QPointF endPos, bool bShiftPress, bool bALtPress);

    void itemPropertyChange(CGraphicsItem *item, QPen pen, QBrush brush, bool bPenChange, bool bBrushChange);

    void itemPolygonPointChange(CGraphicsPolygonItem *item, int oldNum);

    void itemPolygonalStarPointChange(CGraphicsPolygonalStarItem *item, int oldNum, int oldRadius);

    void itemBlurChange(int blurWidth, EBlurEffect effect);

    void signalDoCut(QRectF);

    void signalUpdateCutSize();

public slots:

    void picOperation(int enumstyle);

    void slotSelectionChange();

protected:

    virtual void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent) Q_DECL_OVERRIDE;
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent) Q_DECL_OVERRIDE;
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent) Q_DECL_OVERRIDE;
    virtual void drawItems(QPainter *painter, int numItems,
                           QGraphicsItem *items[],
                           const QStyleOptionGraphicsItem options[],
                           QWidget *widget = nullptr) Q_DECL_OVERRIDE;



};

#endif // CDRAWSCENE_H
