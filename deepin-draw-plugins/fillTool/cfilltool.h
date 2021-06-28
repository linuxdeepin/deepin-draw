#ifndef CFILLTOOL_H
#define CFILLTOOL_H
#include "idrawtool.h"


#ifdef __cplusplus
extern "C" {

#endif
__attribute__((visibility("default"))) IDrawTool *creatTool();

#ifdef __cplusplus
}
#endif

class CFillTool: public IDrawTool
{
    Q_OBJECT
public:
    CFillTool();

    /**
     * @brief toolButton 定义工具的激活按钮
     */
    QAbstractButton *initToolButton() override;

    DrawAttribution::SAttrisList attributions() override;

    CGraphicsItem *creatItem(CDrawToolEvent *event, ITERecordInfo *pInfo) override {return nullptr;}

    int  minMoveUpdateDistance() override;

    int  decideUpdate(CDrawToolEvent *event, ITERecordInfo *pInfo) override;

    void toolStart(CDrawToolEvent *event, ITERecordInfo *pInfo) override;

    void toolUpdate(CDrawToolEvent *event, ITERecordInfo *pInfo) override;

    void toolFinish(CDrawToolEvent *event, ITERecordInfo *pInfo) override;

    int  allowedMaxTouchPointCount() override {return 3;}

    bool returnToSelectTool(CDrawToolEvent *event, IDrawTool::ITERecordInfo *pInfo) override;
};


#endif // CFILLTOOL_H
