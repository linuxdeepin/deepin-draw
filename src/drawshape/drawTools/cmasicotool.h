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

class JDynamicLayer;
class IBlurTool: public IDrawTool
{
    Q_OBJECT
public:
    IBlurTool(QObject *parent = nullptr);
    DrawAttribution::SAttrisList attributions() override;
    QCursor cursor() const override;

protected:
    QAbstractButton *initToolButton() override;
    void  registerAttributionWidgets() override;

    void toolStart(CDrawToolEvent *event, ITERecordInfo *pInfo) override;
    int  decideUpdate(CDrawToolEvent *event, ITERecordInfo *pInfo) override;
    void toolUpdate(CDrawToolEvent *event, ITERecordInfo *pInfo) override;
    void toolFinish(CDrawToolEvent *event, ITERecordInfo *pInfo) override;
    bool returnToSelectTool(CDrawToolEvent *event, IDrawTool::ITERecordInfo *pInfo) override;
    bool isEnable(PageView *pView) override;
    /**
     * @brief minMoveUpdateDistance　创建图元时最小移动距离
     */
    int minMoveUpdateDistance() override;

    int  allowedMaxTouchPointCount() override;
    void onStatusChanged(EStatus oldStatus, EStatus nowStatus) override;

    bool eventFilter(QObject *o, QEvent *e) override;

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
    void saveZ(PageScene *scene);

    /**
     * @brief restoreZ 还原当前场景下图元的z值,必须与saveZ成对出现
     */
    void restoreZ();

    /**
     * @brief saveItemZValue 缓存图元的z值
     */
    void saveItemZValue(CGraphicsItem *pItem);


private:
    QMap<PageScene *, JDynamicLayer *> _layers;

    struct LayerSaveInfo {
        LayerSaveInfo(qreal zz = 0, bool b = false): z(zz), blocked(b) {}
        qreal z;
        bool  blocked;
    };
    QMap<JDynamicLayer *, LayerSaveInfo>        _saveZs;

    //该次可进行模糊的图元
    QList<CGraphicsItem *> _blurEnableItems;

    //改次进行了模糊操作的图元
    QSet<CGraphicsItem *> _bluringItemsSet;

    bool _pressedPosBlurEnable = false;

    QMap<CGraphicsItem *, qreal> _tempZs;

    CGraphicsItem *_pLastTopItem = nullptr;
};

#endif // CMASICOTOOL_H
