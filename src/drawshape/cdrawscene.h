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

#include "drawshape/globaldefine.h"
#include "sitemdata.h"

class QGraphicsSceneMouseEvent;
class QKeyEvent;
class CGraphicsRectItem;
class CGraphicsPolygonItem;
class CGraphicsPolygonalStarItem;
class CGraphicsPenItem;
class CGraphicsLineItem;
class CGraphicsMasicoItem;
class CGraphicsItemGroup;
class CGraphicsItemHighLight;
class CDrawParamSigleton;
class CGraphicsView;

class CDrawScene : public QGraphicsScene
{
    Q_OBJECT
    friend CSelectTool;

public:
    /**
     * @brief CDrawScene 构造函数
     * @param parent
     */
    explicit CDrawScene(CGraphicsView *view = nullptr,
                        const QString &uuid = "",
                        bool isModified = false);
    ~CDrawScene() override;
    /**
     * @brief initScene 初始化一个新的场景
     */
    void initScene();

    /**
     * @brief drawView 返回视图指针
     */
    CGraphicsView *drawView();

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
     * @brief mouseEvent 鼠标事件
     * @param mouseEvent
     */
    void mouseEvent(QGraphicsSceneMouseEvent *mouseEvent);

    /**
     * @brief drawBackground
     * @param painter
     * @param rect
     */
    void drawBackground(QPainter *painter, const QRectF &rect) override;
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
    /**
     * @brief doCutScene 进行裁剪
     */
    void doCutScene();
    /**
     * @brief doAdjustmentScene 自适应
     * @param rect scene大小
     */
    void doAdjustmentScene(QRectF rect, CGraphicsItem *item = nullptr);

    void setItemDisable(bool canSelecte);

    void blockUpdateBlurItem(bool b);
    /**
     * @brief updateBlurItem 刷新
     * @param changeItem 引起变化的图元
     */
    void updateBlurItem(QGraphicsItem *changeItem = nullptr);

    void switchTheme(int type);

    CGraphicsItemGroup *getItemsMgr() const;

    /**
     * @brief getCDrawParam　获取绘制数据
     */
    CDrawParamSigleton *getDrawParam();

    bool getModify() const;
    void setModify(bool isModify);

    static bool isBussizeItem(QGraphicsItem *pItem);
    static bool isBussizeHandleNodeItem(QGraphicsItem *pItem);
    static bool isBzAssicaitedItem(QGraphicsItem *pItem);
    static bool isNormalGroupItem(QGraphicsItem *pItem);

    CGraphicsItem *getAssociatedBzItem(QGraphicsItem *pItem);

    void clearMrSelection();
    void selectItem(QGraphicsItem *pItem, bool onlyBzItem = true, bool updateAttri = true, bool updateRect = true);
    void notSelectItem(QGraphicsItem *pItem, bool updateAttri = true, bool updateRect = true);
    void selectItemsByRect(const QRectF &rect, bool replace = true, bool onlyBzItem = true);
    void updateMrItemBoundingRect();

    QList<QGraphicsItem *> getBzItems(const QList<QGraphicsItem *> &items = QList<QGraphicsItem *>());

    //EDesSort降序（第一个为最顶层）   EAesSort升序（第一个为最底层）
    enum ESortItemTp { EDesSort,
                       EAesSort,
                       ESortCount
                     };
    void sortZ(QList<QGraphicsItem *> &list, ESortItemTp tp = EDesSort);
    QList<QGraphicsItem *> returnSortZItems(const QList<QGraphicsItem *> &list, ESortItemTp tp = EDesSort);

    //penalgor表示Penetration algorithm，穿透算法
    CGraphicsItem *topBzItem(const QPointF &pos, bool penalgor = true, int IncW = 0);

    CGraphicsItem *firstBzItem(const QList<QGraphicsItem *> &items,
                               bool haveDesSorted = false);

    /*
     *  penalgor表示Penetration algorithm，穿透算法，需要考虑穿透算法过滤
        isBzItem表示必须返回的顶层图元需要是业务图元。
        seeNodeAsBzItem 将业务图元的子图元node也当做是业务图元的一部分，如果为true将返回其父业务图元
        filterMrAndHightLight 过滤掉不参与考虑的图元
    */
    QGraphicsItem *firstItem(const QPointF &pos,
                             const QList<QGraphicsItem *> &items = QList<QGraphicsItem *>(),
                             bool isListDesSorted = true,
                             bool penalgor = false,
                             bool isBzItem = false,
                             bool seeNodeAsBzItem = false,
                             bool filterMrAndHightLight = true,
                             int incW = 0);

    //第一个非Mr的item
    QGraphicsItem *firstNotMrItem(const QList<QGraphicsItem *> items);

    void moveItems(const QList<QGraphicsItem *> &itemlist, const QPointF &move);

    /**
     * @brief setMaxZValue 记录图元最大z值
     * @param zValue 图元z值
     */
    void setMaxZValue(qreal zValue);

    /**
     * @brief getMaxZValue　获取图元最大z值
     */
    qreal getMaxZValue();

    /**
     * @brief 阻止响应鼠标移动事件
     */
    void blockMouseMoveEvent(bool b);

    /**
     * @brief 当前是否处于阻止响应鼠标移动事件
     */
    bool isBlockMouseMoveEvent();

    /**
     * @brief recordSecenInfoToCmd 记录场景的对应类型的属性信息(当前仅支持组合情况的变化,因为其他需要带参数表示变化的对象是谁)
     * @param exptype 场景改变的是什么类型,可去CSceneUndoRedoCommand查看
     */
    void recordSecenInfoToCmd(int exptype, EVarUndoOrRedo varFor);

    /**
     * @brief recordItemsInfoToCmd 记录图元的信息
     */
    void recordItemsInfoToCmd(const QList<CGraphicsItem *> &items, EVarUndoOrRedo varFor, bool clearInfo = true);


    /**
     * @brief 完成记录执行操作
     */
    void finishRecord(bool doRedoCmd = false);

    /**
     * @brief addItem 添加图元
     * @param int [type] 图元类型
     * @return 返回成功与否
     */
    CGraphicsItem *addItemByType(const int &itemType);

    /**
     * @brief isGroupable 是否可以创建一个组合(默认是判断当前场景选中情况下是否可以进行组合)
     */
    bool isGroupable(const QList<CGraphicsItem *> &pBzItems = QList<CGraphicsItem *>());

    /**
     * @brief getManageGroup 获取到传入的图元的共同的顶层组合(如果不存在,那么返回空)
     */
    CGraphicsItemGroup *getSameTopGroup(const QList<CGraphicsItem *> &pBzItems);

    /**
     * @brief creatGroup 创建一个组合
     */
    CGraphicsItemGroup *creatGroup(const QList<CGraphicsItem *> &pBzItems = QList<CGraphicsItem *>(),
                                   bool pushUndo = false);

    /**
     * @brief creatGroup 取消当前选中的组合
     */
    void cancelGroup(CGraphicsItemGroup *pGroup = nullptr,
                     bool pushUndo = false);

    /**
     * @brief destoryGroup 销毁一个组合
     */
    void destoryGroup(CGraphicsItemGroup *pGroup, bool deleteIt = false, bool pushUndo = false);

    /**
     * @brief destoryAllGroup 销毁所有组合
     */
    void destoryAllGroup(bool deleteIt = false, bool pushUndo = false);

    /**
     * @brief getGroup 通过一个业务图元获取到它所处的组合图元(返回空证明不在某个组合图元中,即不处于组合状态)
     */
    CGraphicsItemGroup *getGroup(CGraphicsItem *pBzItem);

    /**
     * @brief bzGroups 返回当前场景下的所有组合情况
     */
    QList<CGraphicsItemGroup *> bzGroups();

signals:
    /**
     * @brief signalAttributeChanged 发送属性栏更改的信号
     * @param flag
     * @param primitiveType 图元类型
     */
    void signalAttributeChanged(bool flag, int primitiveType);
    /**
     * @brief signalChangeToSelect 发送工具栏切换为选择的信号
     * @param showTitle 是否显示select工具的顶层标题(也就是"画板"标题文字)
     */
    void signalChangeToSelect(bool showTitle = false);
    /**
     * @brief signalQuitCutMode 退出裁剪模式
     */
    void signalQuitCutAndChangeToSelect();

    /**
     * @brief itemAdded 增加图元
     * @param item
     */
    void itemAdded(QGraphicsItem *item, bool pushToStack = true);

    /**
     * @brief signalUpdateCutSize 更新裁剪的大小
     */
    void signalUpdateCutSize();

    /**
     * @brief signalUpdateTextFont 更新文字字体
     */
    void signalUpdateTextFont();

    /**
     * @brief signalSceneCut 裁剪场景
     * @param newRect
     */
    void signalSceneCut(QRectF newRect);

    /**
     * @brief signalIsModify 是否更改
     * @param newRect
     */
    void signalIsModify(bool isModify);

public slots:

    /**
     * @brief changeMouseShape 切换鼠标形状
     * @param type
     */
    void changeMouseShape(EDrawToolMode type);

    /**
     * @brief setDrawForeground 是否绘制前景元素(前景主要绘制了框选矩形,高亮,模糊图元截图时不需要绘制这些)
     */
    void setDrawForeground(bool b);

    /**
     * @brief setDrawForeground 是否绘制前景元素
     */
    bool isDrawedForeground();


    void doLeave();

protected:
    /**
     * @brief mousePressEvent 鼠标按下事件
     * @param mouseEvent
     */
    void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent) override;

    /**
     * @brief mouseMoveEvent 鼠标移动事件
     * @param mouseEvent
     */
    void mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent) override;

    /**
     * @brief mouseReleaseEvent 鼠标放开事件
     * @param mouseEvent
     */
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent) override;

    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *mouseEvent) override;

    bool event(QEvent *event) override;

    /**
     * @brief drawItems 绘制所有图元
     * @param painter
     * @param numItems
     * @param items
     * @param options
     * @param widget
     */
    void drawItems(QPainter *painter, int numItems,
                   QGraphicsItem *items[],
                   const QStyleOptionGraphicsItem options[],
                   QWidget *widget = nullptr) override;

    void drawForeground(QPainter *painter, const QRectF &rect) override;

public:
    void refreshLook(const QPointF &pos = QPointF());
    /*
     * @breaf: clearHighlight 清除高亮图元路径
    */
    void clearHighlight();

    void setHighlightHelper(const QPainterPath &path);

    QPainterPath hightLightPath();


public:

    using CGroupBzItemsTree = CBzGroupTree<CGraphicsItem *>;

    /**
     * @brief getGroupTree  获取pGroup的组合信息(以树结构进行表示,业务图元以CGraphicsItem指针的形式保存)
     * @param fatherGroup   传nullptr表示获取到当前场景下组合信息否则仅收集获取pGroup的信息
     */
    CGroupBzItemsTree getGroupTree(CGraphicsItemGroup *pGroup = nullptr);

    /**
     * @brief getGroupTreeInfo  获取pGroup的组合信息(以树结构进行表示,抽象出所有业务图元的数据,以CGraphicsUnit的数据形式保存)
     * @param fatherGroup   传nullptr表示获取到当前场景下组合信息否则仅收集获取pGroup的信息
     * @param reson         可决定CGraphicsUnit的数据有那些形势
     */
    CGroupBzItemsTreeInfo getGroupTreeInfo(CGraphicsItemGroup *pGroup = nullptr, EDataReason reson = EDuplicate);

    /**
     * @brief loadGroupTree 从组合树中读取到信息直接实现改组合(返回顶层组合)
     * @param info   组合数信息
     */
    CGraphicsItemGroup *loadGroupTree(const CGroupBzItemsTree &info);

    /**
     * @brief loadGroupTreeInfo 从组合树中读取到信息直接实现改组合(返回顶层组合)
     * @param info   组合数信息
     */
    CGraphicsItemGroup *loadGroupTreeInfo(const CGroupBzItemsTreeInfo &info);

private:
    CDrawParamSigleton *m_drawParam;//数据

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
    qreal m_maxZValue;

    CGraphicsItemGroup *m_pGroupItem;

    QList<CGraphicsItemGroup *> m_pGroups;       //正在使用(场景中的)的组合图元

    QList<CGraphicsItemGroup *> m_pCachGroups;   //未被使用(不在场景中的)的组合图元

    bool dbCLicked = false;

    bool blockMouseMoveEventFlag = false;

    bool blockMscUpdate = false;

    bool bDrawForeground = true;

    QPainterPath _highlight;

    /* 文字可编辑光标 */
    QCursor m_textEditCursor;
};

Q_DECLARE_METATYPE(CDrawScene::CGroupBzItemsTree);

#endif // CDRAWSCENE_H
