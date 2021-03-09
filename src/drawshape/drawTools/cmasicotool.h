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
#ifndef CMASICOTOOL_H
#define CMASICOTOOL_H
#include "idrawtool.h"
#include "cgraphicsitem.h"
#include <QPainterPath>

class CMasicoTool : public IDrawTool
{
public:
    CMasicoTool();
    ~CMasicoTool() override;

protected:
    /**
     * @brief toolUpdate 工具执行的刷新
     * @param event 事件
     * @param scene 场景
     */
    void toolCreatItemUpdate(CDrawToolEvent *event, ITERecordInfo *pInfo) override;

    /**
     * @brief toolFinish 工具执行的结束
     * @param event 事件
     * @param scene 场景
     */
    void toolCreatItemFinish(CDrawToolEvent *event, ITERecordInfo *pInfo) override;

    /**
     * @brief toolFinish 创建图元
     * @param event 事件
     * @param scene 场景
     */
    CGraphicsItem *creatItem(CDrawToolEvent *event, ITERecordInfo *pInfo) override;

    /**
     * @brief toolStart　工具开始事件
     * @param event      当次事件信息
     * @param pInfo      记录信息
     */
    void toolStart(CDrawToolEvent *event, ITERecordInfo *pInfo) override;


    /**
     * @brief toolStart　判断工具活跃类型
     * @param event      当次事件信息
     * @param pInfo      记录信息
     */
    int decideUpdate(CDrawToolEvent *event, ITERecordInfo *pInfo) override;

    /**
     * @brief toolStart　 工具刷新事件
     * @param event       当次事件信息
     * @param pInfo       记录信息
     */
    void toolUpdate(CDrawToolEvent *event, ITERecordInfo *pInfo) override;

    /**
     * @brief toolStart　工具结束事件
     * @param event      当次事件信息
     * @param pInfo      记录信息
     */
    void toolFinish(CDrawToolEvent *event, ITERecordInfo *pInfo) override;

    /**
     * @brief painter　绘制更多的内容（用于绘制框选矩形和高亮路径）
     * @param painter  绘制指针
     * @param rect     矩形大小
     * @param scene    场景指针
     */
    void drawMore(QPainter *painter, const QRectF &rect, CDrawScene *scene) override;


    /**
     * @brief returnToSelectTool 工具执行的结束
     * @param event 事件
     * @param pInfo 额外信息
     */
    bool returnToSelectTool(CDrawToolEvent *event, IDrawTool::ITERecordInfo *pInfo) override;

    /**
     * @brief isEnable 工具当前是否可用
     */
    bool isEnable(CGraphicsView *pView) override;

private:
    /**
     * @brief isBlurEnable 工具执行的结束
     * @param pItem 当前图元
     */
    bool isBlurEnable(const CGraphicsItem *pItem);

    /**
     * @brief getBlurEnableItems 获取到能进行模糊的图元
     * @param pItem 当前图元 如果pItem是一个基本业务图元,那么直接判断它是否可模糊,如果是一个组合那么判断其组合内是否有可模糊的图元
     */
    QList<CGraphicsItem *> getBlurEnableItems(const CGraphicsItem *pItem);

    /**
     * @brief saveZ 保存当前场景下图元的z值,必须与restoreZ成对出现
     */
    void saveZ(CDrawScene *scene);

    /**
     * @brief restoreZ 还原当前场景下图元的z值,必须与saveZ成对出现
     */
    void restoreZ();

    /**
     * @brief saveItemZValue 缓存图元的z值
     */
    void saveItemZValue(CGraphicsItem *pItem);

private:
    bool _pressedPosBlurEnable = false;

    qreal   _zTemp = 0;        //保存图片模糊之前的Z值
    QCursor _blurCursor;

    QMap<CGraphicsItem *, qreal> _tempZs;

    CGraphicsItem *_pLastTopItem = nullptr;

    //该次可进行模糊的图元
    QList<CGraphicsItem *> _blurEnableItems;

    //改次进行了模糊操作的图元
    QSet<CGraphicsItem *> _bluringItemsSet;
};

#endif // CMASICOTOOL_H
