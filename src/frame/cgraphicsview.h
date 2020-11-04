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

#include "csizehandlerect.h"
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
    explicit CGraphicsView(DWidget *parent = nullptr);

    /**
     * @brief 缩放中心的枚举值，
     * ESceneCenter   表示以画布中心进行缩放
     * EMousePos      表示以鼠标的位置进行缩放
     * ECustomViewPos 表示自定义缩放位置（当前指viewport上的坐标位置）
     */
    enum EScaleCenter { EViewCenter,
                        ESceneCenter,
                        EMousePos,
                        ECustomViewPos
                      };

    /**
     * @brief zoomOut 放大
     */
    void zoomOut(EScaleCenter center = EViewCenter, const QPoint &viewPos = QPoint());

    /**
     * @brief zoomIn 缩小
     */
    void zoomIn(EScaleCenter center = EViewCenter, const QPoint &viewPos = QPoint());

    /**
     * @brief scale 缩放接口
     * @param scale 缩放比例
     */
    void scale(qreal scale, EScaleCenter center = EViewCenter, const QPoint &viewPos = QPoint());

    /**
     * @brief scale 获取缩放接口
     * @return scale 缩放比例
     */
    qreal getScale();

    /**
     * @brief  scaleWithCenter 以某一个中心进行缩放
     * @return factor 缩放因子
     */
    void scaleWithCenter(qreal factor, const QPoint viewPos = QPoint());

    /**
     * @brief showSaveDDFDialog 显示保存DDF对话框
     */
    void showSaveDDFDialog(bool, bool finishClose = false, const QString &saveFilePath = "");

    /**
     * @brief doSaveDDF保存DDFRR
     */
    void doSaveDDF(bool finishClose = false);

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

    /**
     * @brief getModify　是否被修改过
     */
    bool getModify() const;

    /**
     * @brief setModify　当场景内容有改动时会调用该函数进行是否修改过的标记的设置
     */
    void setModify(bool isModify);

    /**
     * @brief isKeySpacePressed　是否当前页下空格键处于被点下的状态
     */
    bool isKeySpacePressed();

    /**
     * @brief drawScene　绘制的场景指针
     */
    CDrawScene *drawScene();

    /**
     * @brief updateCursorShape　刷新鼠标的样式
     */
    Q_SLOT void updateCursorShape();

    /**
     * @brief setCacheEnable　设置是否启动缓冲绘制()
     * @param enable true:会生成一个位图进行绘制加速 false:取消缓冲
     * @param fruzzCurFrame true:生成的位图是当前场景帧 false:全透明的位图
     */
    void setCacheEnable(bool enable, bool fruzzCurFrame = true);

    /**
     * @brief isCacheEnabled　是否当前是启动了缓冲加速的
     */
    bool isCacheEnabled();

    /**
     * @brief cachedPixmap　当前的缓冲图
     */
    QPixmap &cachedPixmap();

protected:

    /**
     * @brief showEvent 重载实现一些显示之前的初始化
     */
    void showEvent(QShowEvent *event)override;

    /**
     * @brief wheelEvent 鼠标滚轮事件响应函数
     * @param event 鼠标滚轮事件
     */
    virtual void wheelEvent(QWheelEvent *event) override;

    /**
     * @brief contextMenuEvent 右键菜单响应事件函数
     * @param event 右键菜单
     */
    void contextMenuEvent(QContextMenuEvent *event) override;

    /**
     * @brief resizeEvent 窗口大小更改响应事件函数
     * @param event
     */
    void resizeEvent(QResizeEvent *event) override;

    /**
     * @brief paintEvent 重绘事件
     * @param event
     */
    void paintEvent(QPaintEvent *event) override;

    /**
     * @brief drawItems 绘制所有图元接口
     * @param painter 绘制的画板指针
     * @param numItems 绘制数
     * @param items 图元
     * @param options 参数
     */
    virtual void drawItems(QPainter *painter, int numItems,
                           QGraphicsItem *items[],
                           const QStyleOptionGraphicsItem options[]) override;

    /**
     * @brief leaveEvent 鼠标离开事件
     * @param event
     */
    virtual void leaveEvent(QEvent *event) override;

    /**
     * @brief dropEvent 拖曳加载文件
     * @param e
     */
    void dropEvent(QDropEvent *e) override;

    /**
     * @brief dragEnterEvent 拖文件进入画板
     * @param event
     */
    void dragEnterEvent(QDragEnterEvent *event) override;

    /**
     * @brief dragMoveEvent 拖拽移动事件
     * @param event
     */
    void dragMoveEvent(QDragMoveEvent *event) override;

    /**
     * @brief enterEvent 拖拽事件
     * @param event
     */
    void enterEvent(QEvent *event) override;

protected:
    void keyPressEvent(QKeyEvent *event)override;
    void keyReleaseEvent(QKeyEvent *event)override;
    bool eventFilter(QObject *o, QEvent *e) override;
    bool viewportEvent(QEvent *event)override;

protected:
    void showMenu(DMenu *pMenu);
    bool gestureEvent(QGestureEvent *event);
protected:
    void panTriggered(QPanGesture *);
    void pinchTriggered(QPinchGesture *);
    void swipeTriggered(QSwipeGesture *);

    QPoint _pressBeginPos;
    QPoint _recordMovePos;
    bool   _spaceKeyPressed = false;
    QCursor _tempCursor;
signals:
    /**
     * @brief signalSetScale 设置缩放信号
     * @param scale 缩放比例
     */
    void signalSetScale(const qreal scale);

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
    void signalPastePixmap(QPixmap pixmap, const QByteArray &srcBytes, bool addUndoRedo = true);

    /**
     * @brief signalLoadDragOrPasteFile 加载或粘贴文件信号
     */
    void signalLoadDragOrPasteFile(QString, bool addUndoRedo = true);

    /**
     * @brief signalSaveFileStatus 保存文件状态信号
     * @param bool 保存状态
     * @param QString 错误字符串
     * @param FileError 错误类型
     */
    void signalSaveFileStatus(const QString &savedFile,
                              bool status,
                              QString errorString,
                              QFileDevice::FileError error,
                              bool needClose);

    /**
     * @brief signalSaveFileNameTooLong 保存文件名字过长信号
     */
    void signalSaveFileNameTooLong();

public slots:
    /**
     * @brief itemAdded
     * @param item
     */
    void itemAdded(QGraphicsItem *item, bool pushToStack);

    /**
     * @brief slotStartLoadDDF 开始加载DDF信号
     * @param rect
     */
    void slotStartLoadDDF(QRectF rect);

    /**
     * @brief slotAddItemFromDDF 添加图元到DDF
     * @param item
     */
    void slotAddItemFromDDF(QGraphicsItem *item, bool pushToStack = true);

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

    /**
     * @bref: updateSelectedItemsAlignment 更新选中图元的对齐方式
     * @param: Qt::AlignmentFlag align 对齐方式
     */
    void updateSelectedItemsAlignment(Qt::AlignmentFlag align);

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
     * @brief slotSetTextAlignment 设置文字对齐方式
     */
    void slotSetTextAlignment(const Qt::Alignment &align);

    /**
     * @brief slotOnTextUndo  文字撤消
     */
    void slotOnTextUndo();

    /**
     * @brief slotOnTextRedo 文字重做
     */
    void slotOnTextRedo();

    /**
     * @brief slotOnTextDelete 文字删除
     */
    void slotOnTextDelete();

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

    DMenu *m_layerMenu;             //图层菜单
    QAction *m_oneLayerUpAct;       //向上一层
    QAction *m_oneLayerDownAct;     //向下一层
    QAction *m_bringToFrontAct;     //置于最顶层
    QAction *m_sendTobackAct;       //置于最底层

    DMenu *m_alignMenu;
    QAction *m_itemsLeftAlign;      //左对齐
    QAction *m_itemsHCenterAlign;   //水平居中对齐
    QAction *m_itemsRightAlign;     //右对齐
    QAction *m_itemsTopAlign;       //顶对齐
    QAction *m_itemsVCenterAlign;   //垂直居中对齐
    QAction *m_itemsBottomAlign;    //底对齐
    QAction *m_itemsHEqulSpaceAlign;//水平等间距对齐
    QAction *m_itemsVEqulSpaceAlign;//垂直等间距对齐

    QAction *m_viewZoomInAction;  // 缩小快捷键
    QAction *m_viewZoomOutAction; // 放大快捷键 ctrl + +
    QAction *m_viewZoomOutAction1; // 放大快捷键 ctrl + =
    QAction *m_viewOriginalAction;


    QAction *m_cutScence;          //裁剪

    //文字图元右键菜单
    DMenu *m_textMenu;                      //文字菜单
    QAction *m_textCutAction;            //文字剪切
    QAction *m_textCopyAction;          //文字复制
    QAction *m_textPasteAction;         //文字粘贴
    QAction *m_textSelectAllAction;  //文字全选
    QAction *m_textDeleteAction;      //文字删除
    QAction *m_textUndoAct;              //文字撤销
    QAction *m_textRedoAct;              //文字重做
    QAction *m_textLeftAlignAct;      //文字左对齐
    QAction *m_textRightAlignAct;   //文字右对齐
    QAction *m_textCenterAlignAct; //文字水平垂直居中对齐(目前Qt只支持水平方向的对齐)


    QUndoStack *m_pUndoStack;
    bool m_visible;

    CDDFManager *m_DDFManager;

    bool m_isShowContext;
    bool m_isStopContinuousDrawing;

    QList<QGraphicsItem *> m_loadFromDDF; //从DDF中加载的图圆


    bool    _cacheEnable = false;
    QPixmap _cachePixmap;

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

    /**
    * @bref: getValidSelectedItems 获取当前选中的有效图元
    * @return: QList<CGraphicsItem *> 有效图元集合
    */
    QList<CGraphicsItem *> getSelectedValidItems();

    /**
     * @brief getCouldPaste 判断当前是否可粘贴
     * @return
     */
    bool getCouldPaste();


    friend class CDrawScene;
};

#endif // CGRAPHICSVIEW_H
