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
#ifndef CGRAPHICSVIEW_H
#define CGRAPHICSVIEW_H

#include "drawshape/csizehandlerect.h"
#include "drawshape/globaldefine.h"

#include "widgets/cmenu.h"
#include <DGraphicsView>
#include <QGestureEvent>

DWIDGET_USE_NAMESPACE

class QUndoStack;
class QUndoCommand;
class CDDFManager;
class CGraphicsItem;
class CGraphicsRectItem;
class CGraphicsPolygonItem;
class CGraphicsPolygonalStarItem;
class CGraphicsPenItem;
class CMenu;
class CGraphicsLineItem;
class CGraphicsMasicoItem;
class CDrawParamSigleton;
class CDrawScene;
/**
 * @brief The CGraphicsView class 图元显示VIEW 类
 *
 */
class CGraphicsView : public DGraphicsView
{
    Q_OBJECT
public:
    /**
     * @brief CGraphicsView 构造函数
     * @param parent
     */
    CGraphicsView(DWidget *parent = nullptr);
    /**
     * @brief zoomOut 放大
     */
    void zoomOut();

    /**
     * @brief zoomIn 缩小
     */
    void zoomIn();

    /**
     * @brief scale 缩放接口
     * @param scale 缩放比例
     */
    void scale(qreal scale);

    /**
     * @brief scale 获取缩放接口
     * @return scale 缩放比例
     */
    qreal getScale();

    /**
     * @brief showSaveDDFDialog 显示保存DDF对话框
     */
    void showSaveDDFDialog(bool);

    /**
     * @brief doSaveDDF保存DDFRR
     */
    void doSaveDDF();

    /**
     * @brief 返回当前的业务scene
     */
    CDrawScene *drawScene();

    /**
     * @brief setContextMenuAndActionEnable 设置菜单项是否可用
     * @param enable
     */
    void setContextMenuAndActionEnable(bool enable);

    /**
     * @brief importData 导入数据
     * @param path 数据路径
     * @param isOpenByDDF 是否是DDF数据 true: 是 false: 否
     */
    void importData(const QString &path, bool isOpenByDDF = false);

    /**
     * @brief disableCutShortcut 禁用裁剪
     */
    void disableCutShortcut(bool);

    /**
     * @brief getCDrawParam　获取绘制数据
     */
    CDrawParamSigleton *getDrawParam();
    /**
     * @brief pushUndoStack　入撤销栈
     */
    void pushUndoStack(QUndoCommand *cmd);

    bool getModify() const;
    void setModify(bool isModify);

    void renderScenePixmap();

protected:
    /**
     * @brief wheelEvent 鼠标滚轮事件响应函数
     * @param event 鼠标滚轮事件
     */
    virtual void wheelEvent(QWheelEvent *event) Q_DECL_OVERRIDE;

    /**
     * @brief contextMenuEvent 右键菜单响应事件函数
     * @param event 右键菜单
     */
    void contextMenuEvent(QContextMenuEvent *event) Q_DECL_OVERRIDE;

    /**
     * @brief resizeEvent 窗口大小更改响应事件函数
     * @param event
     */
    void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;

    /**
     * @brief paintEvent 重绘事件
     * @param event
     */
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;

    /**
     * @brief drawItems 绘制所有图元接口
     * @param painter 绘制的画板指针
     * @param numItems 绘制数
     * @param items 图元
     * @param options 参数
     */
    virtual void drawItems(QPainter *painter, int numItems,
                           QGraphicsItem *items[],
                           const QStyleOptionGraphicsItem options[]) Q_DECL_OVERRIDE;

    /**
     * @brief leaveEvent 鼠标离开事件
     * @param event
     */
    virtual void leaveEvent(QEvent *event) Q_DECL_OVERRIDE;

    /**
     * @brief dropEvent 拖曳加载文件
     * @param e
     */
    void dropEvent(QDropEvent *e) Q_DECL_OVERRIDE;

    /**
     * @brief dragEnterEvent 拖文件进入画板
     * @param event
     */
    void dragEnterEvent(QDragEnterEvent *event) Q_DECL_OVERRIDE;

    /**
     * @brief dragMoveEvent 拖拽移动事件
     * @param event
     */
    void dragMoveEvent(QDragMoveEvent *event) Q_DECL_OVERRIDE;

    /**
     * @brief enterEvent 拖拽事件
     * @param event
     */
    virtual  void enterEvent(QEvent *event) Q_DECL_OVERRIDE;

    //virtual QPainter *sharedPainter() const Q_DECL_OVERRIDE;
    bool viewportEvent(QEvent *event)Q_DECL_OVERRIDE;
    bool gestureEvent(QGestureEvent *event);
    void panTriggered(QPanGesture *gesture);
    void pinchTriggered(QPinchGesture *gesture);
    void swipeTriggered(QSwipeGesture *gesture);
signals:
    /**
     * @brief signalSetScale 设置缩放信号
     * @param scale 缩放比例
     */
    void signalSetScale(const qreal scale);

    /**
     * @brief signalImportPicture 导入图片信号
     * @param path
     */
    void signalImportPicture(QString path);

    /**
     * @brief signalTransmitContinueDoOtherThing 传递继续做事的信号
     */
    void signalTransmitContinueDoOtherThing();

    /**
     * @brief singalTransmitEndLoadDDF 传输停止加载DDF信号
     */
    void singalTransmitEndLoadDDF();

    /**
     * @brief signalPastePixmap 粘贴图片
     * @param pixmap
     */
    void signalPastePixmap(QPixmap pixmap);

    /**
     * @brief signalLoadDragOrPasteFile 加载或粘贴文件信号
     */
    void signalLoadDragOrPasteFile(QString);

    /**
     * @brief signalSaveFileStatus 保存文件状态信号
     * @param bool 保存状态
     * @param QString 错误字符串
     * @param FileError 错误类型
     */
    void signalSaveFileStatus(bool, QString, QFileDevice::FileError);

    /**
     * @brief signalSaveFileNameTooLong 保存文件名字过长信号
     */
    void signalSaveFileNameTooLong();

public slots:

    /**
     * @brief itemMoved
     * @param item
     * @param newPosition
     */
    void itemMoved(QGraphicsItem *item, const QPointF &newPosition );

    /**
     * @brief itemAdded
     * @param item
     */
    void itemAdded(QGraphicsItem *item );

    /**
     * @brief itemRotate
     * @param item
     * @param newAngle
     */
    void itemRotate(QGraphicsItem *item, const qreal newAngle );

    /**
     * @brief itemResize
     * @param item
     * @param handle
     * @param beginPos
     * @param endPos
     * @param bShiftPress
     * @param bALtPress
     */
    void itemResize(CGraphicsItem *item, CSizeHandleRect::EDirection handle, QPointF beginPos, QPointF endPos, bool bShiftPress, bool bALtPress);

    /**
     * @brief itemPropertyChange
     * @param item
     * @param pen
     * @param brush
     * @param bPenChange
     * @param bBrushChange
     */
    void itemPropertyChange(CGraphicsItem *item, QPen pen, QBrush brush, bool bPenChange, bool bBrushChange);

    /**
    * @brief itemPropertyChange
    * @param item
    * @param xRedius
    * @param bChange
    */
    void itemRectXRediusChange(CGraphicsRectItem *item, int xRedius, bool bChange);

    /**
     * @brief itemPolygonPointChange
     * @param item
     * @param newNum
     */
    void itemPolygonPointChange(CGraphicsPolygonItem *item, int newNum);

    /**
     * @brief itemPolygonalStarPointChange
     * @param item
     * @param newNum
     * @param newRadius
     */
    void itemPolygonalStarPointChange(CGraphicsPolygonalStarItem *item, int newNum, int newRadius);

    /**
     * @brief itemPenTypeChange
     * @param item
     * @param newType
     */
    void itemPenTypeChange(CGraphicsPenItem *item, int newType);

    /**
     * @brief itemLineTypeChange
     * @param item
     * @param newType
     */
    void itemLineTypeChange(CGraphicsLineItem *item, ELineType newStartType, ELineType newOldType);

    /**
     * @brief itemBlurChange
     * @param item
     * @param blurWidth
     * @param effect
     */
    void itemBlurChange(CGraphicsMasicoItem *item, int blurWidth, int effect);

    /**
     * @brief slotStopContinuousDrawing 停止或继续绘制信号
     */
    void slotStopContinuousDrawing();

    /**
     * @brief slotStartLoadDDF 开始加载DDF信号
     * @param rect
     */
    void slotStartLoadDDF(QRectF rect);

    /**
     * @brief slotAddItemFromDDF 添加图元到DDF
     * @param item
     */
    void slotAddItemFromDDF(QGraphicsItem *item );

    /**
     * @brief slotQuitCutMode 退出裁剪模式
     */
    void slotQuitCutMode();

    /**
     * @brief clearScene 清除场景
     */
    void clearScene();

    /**
     * @brief itemSceneCut 裁剪画板
     * @param newRect 裁剪的位置大小
     */
    void itemSceneCut(QRectF newRect);

public slots:

    /**
     * @brief slotOnCut 剪切图元
     */
    void slotOnCut();

    /**
     * @brief slotOnCopy 复制图元
     */
    void slotOnCopy();

    /**
     * @brief slotOnPaste 粘贴图元
     */
    void slotOnPaste();

    /**
     * @brief slotOnSelectAll 全选图元
     */
    void slotOnSelectAll();

    /**
     * @brief slotOnDelete 删除图元
     */
    void slotOnDelete();

    /**
     * @brief slotOneLayerUp 图元向上一层
     */
    void slotOneLayerUp();

    /**
     * @brief slotOneLayerDown 图元向下一层
     */
    void slotOneLayerDown();

    /**
     * @brief slotBringToFront 置顶图元
     */
    void slotBringToFront();

    /**
     * @brief slotSendTobackAct 置底图元
     */
    void slotSendTobackAct();

    /**
     * @brief slotDoCutScene 裁剪场景
     */
    void slotDoCutScene();

    /**
     * @brief slotOnTextCut 剪切文字
     */
    void slotOnTextCut();

    /**
     * @brief slotOnTextCopy 复制文字
     */
    void slotOnTextCopy();

    /**
     * @brief slotOnTextPaste 粘贴文字
     */
    void slotOnTextPaste();

    /**
     * @brief slotOnTextSelectAll 全选文字
     */
    void slotOnTextSelectAll();

    /**
     * @brief slotOnTextTopAlignment 文字顶对齐
     */
    void slotOnTextTopAlignment();

    /**
     * @brief slotOnTextRightAlignment 文字右对齐
     */
    void slotOnTextRightAlignment();

    /**
     * @brief slotOnTextLeftAlignment 文字左对齐
     */
    void slotOnTextLeftAlignment();

    /**
     * @brief slotOnTextCenterAlignment 文字中心对齐
     */
    void slotOnTextCenterAlignment();

    /**
     * @brief slotOnTextUndo  文字撤消
     */
    void slotOnTextUndo();

    /**
     * @brief slotOnTextRedo 文字重做
     */
    void slotOnTextRedo();

    /**
     * @brief slotRestContextMenuAfterQuitCut 退出裁剪重置右键菜单
     */
    void slotRestContextMenuAfterQuitCut();

    /**
     * @brief slotViewZoomIn 缩小
     */
    void slotViewZoomIn();

    /**
     * @brief slotViewZoomOut 放大
     */
    void slotViewZoomOut();

    /**
     * @brief slotViewOrignal 缩放到原始大小
     */
    void slotViewOrignal();

private:
    qreal m_scale; //记录当前缩放

    CMenu *m_contextMenu;//右键菜单
    QAction *m_cutAct;              //剪切
    QAction *m_copyAct;             //拷贝
    QAction *m_pasteAct;            //粘贴
    QAction *m_selectAllAct;        //全选
    QAction *m_deleteAct;           //删除
    QAction *m_undoAct;             //撤销
    QAction *m_redoAct;             //重做
    QAction *m_oneLayerUpAct;       //向上一层
    QAction *m_oneLayerDownAct;     //向下一层
    QAction *m_bringToFrontAct;     //置于最顶层
    QAction *m_sendTobackAct;       //置于最底层
//    QAction *m_leftAlignAct;
//    QAction *m_topAlignAct;
//    QAction *m_rightAlignAct;
//    QAction *m_centerAlignAct;

    QAction *m_viewZoomInAction;
    QAction *m_viewZoomOutAction;
    QAction *m_viewOriginalAction;


    QAction *m_cutScence;          //裁剪

    ///文字图元右键菜单
    DMenu *m_textMenu;
    QAction *m_textCutAction;
    QAction *m_textCopyAction;
    QAction *m_textPasteAction;
    QAction *m_textSelectAllAction;
    QAction *m_textLeftAlignAct;
    QAction *m_textTopAlignAct;
    QAction *m_textRightAlignAct;
    QAction *m_textCenterAlignAct;
    QAction *m_textUndoAct;             //文字撤销
    QAction *m_textRedoAct;             //文字重做
    ///

    QUndoStack *m_pUndoStack;
    bool m_visible;

    CDDFManager *m_DDFManager;

    bool m_isShowContext;
    bool m_isStopContinuousDrawing;

private:
    /**
     * @brief initContextMenu 初始化系统右键菜单
     */
    void initContextMenu();

    /**
     * @brief initContextMenuConnection 初始化菜单槽函数
     */
    void initContextMenuConnection();

    /**
     * @brief initTextContextMenu 初始化文字菜单
     */
    void initTextContextMenu();

    /**
     * @brief initTextContextMenuConnection 初始化文字菜单槽函数
     */
    void initTextContextMenuConnection();

    /**
     * @brief initConnection 初始化槽函数
     */
    void initConnection();

    /**
     * @brief canLayerUp 是否可以向上移动图元
     * @return
     */
    bool canLayerUp();

    /**
     * @brief canLayerDown 是否可以向下移动图元
     * @return
     */
    bool canLayerDown();
};

#endif // CGRAPHICSVIEW_H
