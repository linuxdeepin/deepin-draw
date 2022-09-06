// SPDX-FileCopyrightText: 2020 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef CELLIPSETOOL_H
#define CELLIPSETOOL_H
#include "idrawtool.h"

class CEllipseTool : public IDrawTool
{
    Q_OBJECT
public:
    CEllipseTool();
    virtual ~CEllipseTool() override;
    DrawAttribution::SAttrisList attributions() override;

    /**
     * @brief toolButton 定义工具的激活按钮
     */
    QAbstractButton *initToolButton() override;

    QCursor cursor() const override;


protected:
    /**
     * @brief toolUpdate 工具执行的刷新
     * @param event 事件
     * @param scene 场景
     */
    virtual void toolCreatItemUpdate(CDrawToolEvent *event, ITERecordInfo *pInfo) override;

    /**
     * @brief toolFinish 工具执行的结束
     * @param event 事件
     * @param scene 场景
     */
    virtual void toolCreatItemFinish(CDrawToolEvent *event, ITERecordInfo *pInfo) override;

    /**
     * @brief toolFinish 创建图元
     * @param event 事件
     * @param scene 场景
     */
    virtual CGraphicsItem *creatItem(CDrawToolEvent *eventpInfo, ITERecordInfo *pInfo) override;
};

#endif // CELLIPSETOOL_H
