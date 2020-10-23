#ifndef CMASICOTOOL_H
#define CMASICOTOOL_H
#include "idrawtool.h"

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
    virtual CGraphicsItem *creatItem(CDrawToolEvent *event) override;
};

#endif // CMASICOTOOL_H
