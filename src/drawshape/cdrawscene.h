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
#ifndef CDRAWSCENE_H
#define CDRAWSCENE_H
#include "cselecttool.h"
#include <QGraphicsScene>
#include <QPainterPath>
#include <QObject>

#include "drawshape/globaldefine.h"
#include "sitemdata.h"

#include "cgraphicsitemselectedmgr.h"

class QGraphicsSceneMouseEvent;
class QKeyEvent;
class CGraphicsRectItem;
class CGraphicsPolygonItem;
class CGraphicsPolygonalStarItem;
class CGraphicsPenItem;
class CGraphicsLineItem;
//class CGraphicsMasicoItem;
//class CGraphicsItemGroup;
class CGraphicsItemHighLight;
class CDrawParamSigleton;
class CGraphicsView;

extern bool zValueSortDES(QGraphicsItem *info1, QGraphicsItem *info2);
extern bool zValueSortASC(QGraphicsItem *info1, QGraphicsItem *info2);

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
    void resetScene();

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

    /**
     * @brief setItemsActive　设置所有图元是否是活跃的,可操作的(可能的应用场景:裁剪模式下图元都不可操作)
     */
    void setItemsActive(bool canSelecte);

    /**
     * @brief blockUpdateBlurItem　禁止刷新模糊(用于减少模糊的刷新次数,避免多次重复刷新,记得还原)
     */
//    void blockUpdateBlurItem(bool b);

    /**
     * @brief updateBlurItem 刷新模糊
     * @param changeItem 引起变化的图元
     */
    //void updateBlurItem(QGraphicsItem *changeItem = nullptr);

    /**
     * @brief switchTheme　切换主题
     */
    void switchTheme(int type);

    /**
     * @brief getCDrawParam　获取绘制数据
     */
    CDrawParamSigleton *getDrawParam();

    /**
     * @brief isModified　是否被修改过
     */
    bool isModified() const;

    /**
     * @brief setModify　设置场景是否被修改过(发生过变化)
     */
    void setModify(bool isModify);

    /**
     * @brief addCItem　添加图元
     */
    void addCItem(QGraphicsItem *pItem, bool calZ = true);

    /**
     * @brief removeCItem　删除图元
     */
    void removeCItem(QGraphicsItem *pItem, bool del = false);

    /**
     * @brief blockAssignZValue　禁止自动分配给图元z值(在一些直接还原图元z值的情况加入场景时,不需要进行z值分配)
     */
    void blockAssignZValue(bool b);

    /**
     * @brief isBussizeItem　是否是基本业务图元
     */
    static bool isBussizeItem(QGraphicsItem *pItem);

    /**
     * @brief isBussizeHandleNodeItem　是否是基本业务图元的节点图元
     */
    static bool isBussizeHandleNodeItem(QGraphicsItem *pItem);

//    /**
//     * @brief isBzAssicaitedItem　是否是与基本业务图元有关联图元
//     */
//    static bool isBzAssicaitedItem(QGraphicsItem *pItem);

    /**
     * @brief isNormalGroupItem　是否是常规的组合图元
     */
    static bool isNormalGroupItem(QGraphicsItem *pItem);


    /**
     * @brief isDrawItem　是否是画板图元(Normal组合图元+基本业务图元)
     */
    static bool isDrawItem(QGraphicsItem *pItem);

    /**
     * @brief getAssociatedBzItem　得到与pItem有关联的基本业务图元
     */
    CGraphicsItem *getAssociatedBzItem(QGraphicsItem *pItem);

    /**
     * @brief selectGroup　多选组合图元
     */
    CGraphicsItemGroup *selectGroup();

    /**
     * @brief clearSelectGroup　清理选中
     */
    void clearSelectGroup();

    /**
     * @brief selectItem　选择一个图元
     */
    void selectItem(QGraphicsItem *pItem, bool onlyBzItem = true, bool updateAttri = true, bool updateRect = true);

    /**
     * @brief notSelectItem　不选择某个图元
     */
    void notSelectItem(QGraphicsItem *pItem, bool updateAttri = true, bool updateRect = true);

    /**
     * @brief blockSelect锁住选中
     * (为true后调用selectItem或selectItemsByRect函数将没有作用,应用场景一般加载组合树时不期望进行选中操作)
     * (但可以通过获取选择组合selectGroup()->add进行添加的方式进行选中,这种方式不受该标记限制)
     */
    void blockSelect(bool b);

    /**
     * @brief selectItemsByRect　通过一个矩形范围选中图元
     */
    void selectItemsByRect(const QRectF &rect, bool replace = true, bool onlyBzItem = true);

    /**
     * @brief selectItemsByRect　通过一个矩形范围选中图元
     */
    void updateMrItemBoundingRect();


    //EDesSort降序（第一个为最顶层）   EAesSort升序（第一个为最底层）
    enum ESortItemTp {EDesSort, EAesSort, ESortCount};

    /**
     * @brief getBzItems　获取一组图元中的基本业务图元(默认是按照z值降序)
     * @param items 某一组图元,注意:如果为空,那么会获取当前场景下的所有基本业务图元
     */
    QList<CGraphicsItem *> getBzItems(const QList<QGraphicsItem *> &items = QList<QGraphicsItem *>(),
                                      ESortItemTp tp = EDesSort);

    /**
     * @brief getRootItems　获取场景中所有的根图元(包括顶根组合图元),即bzGroup(true) == nullptr的图元
     * @param tp 表示排序类型
     */
    QList<CGraphicsItem *> getRootItems(ESortItemTp tp = EDesSort);

    /**
     * @brief moveBzItemsLayer 设置图元绘制的顺序
     * @param items表示要移动的基本业务图元
     * @param tp表示z要移动的方向
     * @param step要跳动的步数(-1表示到极限),tp为EDownLayer或EUpLayer时有效
     * @param pBaseInGroup 仅当tp为EToGroup才有效,以某一个图元z值为基准进行设置
     */
    void moveBzItemsLayer(const QList<CGraphicsItem *> &items,
                          EZMoveType tp, int step = 1,
                          CGraphicsItem *pBaseInGroup = nullptr,
                          bool pushToStack = false);

//    bool isZMovable(const QList<CGraphicsItem *> &items,
//                    EZMoveType tp, int step = 1,
//                    CGraphicsItem *pBaseInGroup = nullptr);

    /**
     * @brief isCurrentZMovable 判断当前是否可改变图层顺序
     */
    bool isCurrentZMovable(EZMoveType tp, int step = 1,
                           CGraphicsItem *pBaseInGroup = nullptr);

    /**
     * @brief sortZ　通过z值对一组图元进行排序
     * @param list 某一组图元(即是入参也是出参)
     * @param tp 排序方式(升序还是降序)
     */

    template<typename T>
    static void sortZ(QList<T *> &list, ESortItemTp tp = EDesSort)
    {
        auto f = (tp == EAesSort ? zValueSortASC : zValueSortDES);
        // 调用了过时的函数“ qSort”,改用'std :: sort'。
        std::sort(list.begin(), list.end(), f);
    }

    /**
     * @brief returnSortZItems　按照某一升降顺序进行排序
     */
    template<typename T>
    static QList<T *> returnSortZItems(const QList<T *> &list, ESortItemTp tp = EDesSort)
    {
        QList<T *> sorts = list;
        sortZ(sorts, tp);
        return sorts;
    }

    /**
     * @brief topBzItem　获取某一点下的最顶层画板图元
     * @param pos 位置
     * @param penalgor 是否是考虑使用穿透算法过滤
     * @param IncW     以pos为中心作Incw半径矩形,将这个矩形范围内的图元也考虑进来
     */
    CGraphicsItem *topBzItem(const QPointF &pos, bool penalgor = true, int IncW = 0);

    /**
     * @brief firstItem　获取一组图元中的第一个(最顶层)图元
     * @param pos 位置
     * @param items 某一组图元
     * @param isListDesSorted 该组图元items是否是按照z值降序排列好了
     * @param penalgor 是否是考虑使用穿透算法过滤
     * @param isBzItem 返回的图元是否必须是基本业务图元
     * @param seeNodeAsBzItem 是否items中的基本业务图元的节点图元会被当做是这个基本业务图元来看待
     * @param filterMrAndHightLight 过滤掉高亮和多选组合图元
     * @param incW     以pos为中心作incW半径矩形,将这个矩形范围内的图元也考虑进来
     */
    QGraphicsItem *firstItem(const QPointF &pos,
                             const QList<QGraphicsItem *> &items = QList<QGraphicsItem *>(),
                             bool isListDesSorted = true,
                             bool penalgor = false,
                             bool isBzItem = false,
                             bool seeNodeAsBzItem = false,
                             bool filterMrAndHightLight = true,
                             int incW = 0);

// Cppcheck检测函数没有使用到
//    QList<CGraphicsItem *> findBzItems(const QPointF &pos,
//                                       bool seeNodeAsBzItem = false,
//                                       bool filterMrAndHightLight = true,
//                                       int incW = 0);

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
    void recordSecenInfoToCmd(int exptype, EVarUndoOrRedo varFor, const QList<CGraphicsItem *> &items = QList<CGraphicsItem *>());

    /**
     * @brief recordItemsInfoToCmd 记录图元的信息
     */
    void recordItemsInfoToCmd(const QList<CGraphicsItem *> &items, EVarUndoOrRedo varFor, bool clearInfo = true, const QList<CGraphicsItem *> &shoudNotSelectItems = QList<CGraphicsItem *>());

    /**
     * @brief 完成记录执行操作
     */
    void finishRecord(bool doRedoCmd = false);

    /**
     * @brief isGroupable 是否可以创建一个组合(默认是判断当前场景选中情况下是否可以进行组合)
     */
    bool isGroupable(const QList<CGraphicsItem *> &pBzItems = QList<CGraphicsItem *>());

    /**
     * @brief isUnGroupable 框选的内容是否有组合图元可以进行撤销组合
     */
    bool isUnGroupable(const QList<CGraphicsItem *> &pBzItems = QList<CGraphicsItem *>());

    /**
     * @brief getManageGroup 获取到传入的图元的共同的顶层组合(如果不存在,那么返回空)
     * @param pBzItems 待获取的业务图元们
     * @param top为false有意义,如果图元们的组合都是nullptr,那么当sameNullCreatVirGroup为true时,会创建一个虚拟的组合进行统一管理,否则依然返回nullptr,用完记得删除!!!
     * @return 返回共同的组合图元,如果有不同的组合图元返回nullptr
     */
    CGraphicsItemGroup *getSameGroup(const QList<CGraphicsItem *> &pBzItems,
                                     bool top = false,
                                     bool onlyNormal = true,
                                     bool sameNullCreatVirGroup = false);

    /**
     * @brief creatGroup 基于多个图元创建一个组合(会将新创建的组合加入到这个场景中)
     * @param pBzItems 场景内的图元们(这个图元会被加入到新创建的组合中去)
     * @param pushUndo 改操作是否支持撤销还原
     * @param pBzItemZBase 组合创建时,需要指定一个基础z值,以确定组合内基本图元的z值,为nullptr时取组合中z值最小的基本图元
     * @return 返回新创建的组合图元,失败返回nullptr(比如pBzItems的个数小于2,或者其中图元已经处于一个组合下了)
     */
    CGraphicsItemGroup *creatGroup(const QList<CGraphicsItem *> &pBzItems = QList<CGraphicsItem *>(),
                                   int groupType = 0,
                                   bool pushUndo = false,
                                   CGraphicsItem *pBzItemZBase = nullptr,
                                   bool getMinZItemIfNull = false);

    /**
     * @brief creatGroup 复制出一个和参数pGroup一样的组合(前提:该组合必须已经处于scene中,同时新创建的组合会被加入到这个组合的场景中去)
     * @param pGroup 源组合图元
     */
    static CGraphicsItemGroup *copyCreatGroup(CGraphicsItemGroup *pGroup);

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

//    /**
//     * @brief getGroup 通过一个业务图元获取到它所处的组合图元(返回空证明不在某个组合图元中,即不处于组合状态)
//     */
//    CGraphicsItemGroup *getGroup(CGraphicsItem *pBzItem);

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

//    /**
//     * @brief setDrawForeground 是否绘制前景元素
//     */
//    bool isDrawedForeground();


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

//    QPainterPath hightLightPath();
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
     * @param info   组合树信息
     */
    CGraphicsItemGroup *loadGroupTree(const CGroupBzItemsTree &info);

    /**
     * @brief loadGroupTreeInfo 从组合树中读取到信息直接实现改组合(返回顶层组合)
     * @param info   组合树信息
     * @return 返回一个组合图元,这个组合图元以树状结构描述了新添加的图元信息(一般它没有实际意义只是一个抽象描述,用完记得删除)
     */
    CGraphicsItemGroup *loadGroupTreeInfo(const CGroupBzItemsTreeInfo &info, bool notClear = false);

    /**
     * @brief releaseBzItemsTreeInfo 释放组合树咱用的内存
     * @param info   组合树信息
     */
    static void releaseBzItemsTreeInfo(CGroupBzItemsTreeInfo info);
    static void releaseBzItemsTreeInfo(CGroupBzItemsTree info);

private:

    /**
     * @brief moveItemsZDown 将多个图元进行图层的向下移动
     * @param items   图元信息
     * @param step　　要跳动的图层步数(-1表示移动到最底层)
     */
    void moveItemsZDown(const QList<CGraphicsItem *> &items, int step);

    /**
     * @brief moveItemsZDown 将多个图元进行图层的向上移动
     * @param items   图元信息
     * @param step　　要跳动的图层步数(-1表示移动到最上层)
     */
    void moveItemsZUp(const QList<CGraphicsItem *> &items, int step);

    /**
     * @brief sortZBaseOneBzItem 将多个图元组成一个组合时的z值排序算法
     * @param items   图元信息
     * @param pBaseItem 组合z值的基准图元
     */
    void sortZBaseOneBzItem(const QList<CGraphicsItem *> &items, CGraphicsItem *pBaseItem);


    /**
     * @brief initCursor 初始化一些光标
     */
    void initCursor();

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

    CGraphicsItemGroup *m_pSelGroupItem = nullptr;

    QList<CGraphicsItemGroup *> m_pGroups;       //正在使用(场景中的)的组合图元

    QList<CGraphicsItemGroup *> m_pCachGroups;   //未被使用(不在场景中的)的组合图元

    QSet<QGraphicsItem *> m_notInSceneItems;

    bool dbCLicked    = false;
    bool blockZAssign = false;
    bool blockSel     = false;

    bool blockMouseMoveEventFlag = false;

    bool blockMscUpdate = false;

    bool bDrawForeground = true;

    QPainterPath _highlight;

    /* 文字可编辑光标 */
    QCursor m_textEditCursor;
};

Q_DECLARE_METATYPE(CDrawScene::CGroupBzItemsTree);

#endif // CDRAWSCENE_H
