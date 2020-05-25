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
#ifndef CDRAWSCENE_H
#define CDRAWSCENE_H
#include "cselecttool.h"
#include <QGraphicsScene>
#include <QObject>

class QGraphicsSceneMouseEvent;
class QKeyEvent;
class CGraphicsPolygonItem;
class CGraphicsPolygonalStarItem;
class CGraphicsPenItem;
class CGraphicsLineItem;
class CGraphicsMasicoItem;

class CDrawScene : public QGraphicsScene
{
    Q_OBJECT
    friend CSelectTool;

public:
    static CDrawScene *m_pInstance;
    static CDrawScene *GetInstance();

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
     * @brief resetSceneBackgroundBrush  重新设置背景颜色
     */
    void resetSceneBackgroundBrush();
    /**
     * @brief showCutItem 显示裁剪图元
     */
    void showCutItem();
    /**
     * @brief quitCutMode 退出裁剪
     */
    void quitCutMode();
    void doCutScene();

    void setItemDisable(bool canSelecte);

    void textFontFamilyChanged();

    void textFontSizeChanged();

    /**
     * @brief updateBlurItem 刷新
     * @param changeItem 引起变化的图元
     */
    void updateBlurItem(QGraphicsItem *changeItem = nullptr);

    void switchTheme(int type);
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
    void signalQuitCutAndChangeToSelect();

    /**
     * @brief itemMoved 移动
     * @param item
     * @param pos
     */
    void itemMoved(QGraphicsItem *item, QPointF pos);

    /**
     * @brief itemAdded 增加图元
     * @param item
     */
    void itemAdded(QGraphicsItem *item);

    /**
     * @brief itemRotate 旋转图元
     * @param item
     * @param oldAngle
     */
    void itemRotate(QGraphicsItem *item, const qreal oldAngle );

    /**
     * @brief itemResize 更改图元大小
     * @param item
     * @param handle
     * @param beginPos
     * @param endPos
     * @param bShiftPress
     * @param bALtPress
     */
    void itemResize(CGraphicsItem *item, CSizeHandleRect::EDirection handle, QPointF beginPos, QPointF endPos, bool bShiftPress, bool bALtPress);

    /**
     * @brief itemPropertyChange 图元属性修改
     * @param item
     * @param pen
     * @param brush
     * @param bPenChange
     * @param bBrushChange
     */
    void itemPropertyChange(CGraphicsItem *item, QPen pen, QBrush brush, bool bPenChange, bool bBrushChange);

    /**
     * @brief itemPolygonPointChange 多边形边数更改
     * @param item
     * @param oldNum
     */
    void itemPolygonPointChange(CGraphicsPolygonItem *item, int oldNum);

    /**
     * @brief itemPolygonalStarPointChange 星形边数更改
     * @param item
     * @param oldNum
     * @param oldRadius
     */
    void itemPolygonalStarPointChange(CGraphicsPolygonalStarItem *item, int oldNum, int oldRadius);

    /**
     * @brief itemBlurChange 模糊图元属性更改
     * @param item
     * @param effect
     * @param blurWidth
     */
    void itemBlurChange(CGraphicsMasicoItem *item, int effect, int blurWidth);

    /**
     * @brief itemPenTypeChange 画笔图元更改
     * @param item
     * @param oldType
     */
    void itemPenTypeChange(CGraphicsPenItem *item, int oldType);

    /**
     * @brief itemLineTypeChange 线图元属性更改
     * @param item
     * @param type
     */
    void itemLineTypeChange(CGraphicsLineItem *item, int type);

    /**
     * @brief signalUpdateCutSize 更新裁剪的大小
     */
    void signalUpdateCutSize();

    /**
     * @brief signalUpdateTextFont 更新文字字体
     */
    void signalUpdateTextFont();

    /**
     * @brief signalUpdateColorPanelVisible 更新调色板显示
     * @param pos
     */
    void signalUpdateColorPanelVisible(QPoint pos);

    /**
     * @brief signalSceneCut 裁剪场景
     * @param newRect
     */
    void signalSceneCut(QRectF newRect);

public slots:

    /**
     * @brief picOperation 图片操作
     * @param enumstyle 操作方式
     */
    void picOperation(int enumstyle);

    /**
     * @brief drawToolChange 切换绘图工具
     * @param type
     */
    void drawToolChange(int type);

    /**
     * @brief changeMouseShape 切换鼠标形状
     * @param type
     */
    void changeMouseShape(EDrawToolMode type);

    QPixmap &scenPixMap();
    void     renderSelfToPixmap();

protected:

    /**
     * @brief mousePressEvent 鼠标按下事件
     * @param mouseEvent
     */
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent) Q_DECL_OVERRIDE;

    /**
     * @brief mouseMoveEvent 鼠标移动事件
     * @param mouseEvent
     */
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent) Q_DECL_OVERRIDE;

    /**
     * @brief mouseReleaseEvent 鼠标放开事件
     * @param mouseEvent
     */
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent) Q_DECL_OVERRIDE;

    /**
     * @brief drawItems 绘制所有图元
     * @param painter
     * @param numItems
     * @param items
     * @param options
     * @param widget
     */
    virtual void drawItems(QPainter *painter, int numItems,
                           QGraphicsItem *items[],
                           const QStyleOptionGraphicsItem options[],
                           QWidget *widget = nullptr) Q_DECL_OVERRIDE;

    virtual void drawForeground(QPainter *painter, const QRectF &rect) Q_DECL_OVERRIDE;


private:
    /**
     * @brief CDrawScene 构造函数
     * @param parent
     */
    explicit CDrawScene(QObject *parent = nullptr);

private:
    bool m_bIsEditTextFlag;

    QCursor m_drawMouse;
    QCursor m_lineMouse;
    QCursor m_pengatonMouse;
    QCursor m_rectangleMouse;
    QCursor m_roundMouse;
    QCursor m_starMouse;
    QCursor m_triangleMouse;
    QCursor m_textMouse;
    QCursor m_brushMouse;
    QCursor m_blurMouse;

    QPixmap m_scenePixMap;

};

#endif // CDRAWSCENE_H
