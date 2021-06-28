#ifndef CERASER_H
#define CERASER_H
#include "idrawtool.h"

#ifdef __cplusplus
extern "C" {

#endif
__attribute__((visibility("default"))) IDrawTool *creatTool();

#ifdef __cplusplus
}
#endif

class CEraser: public IDrawTool
{
    Q_OBJECT
public:
    CEraser();

    /**
     * @brief toolButton 定义工具的激活按钮
     */
    QAbstractButton *initToolButton() override;

    CGraphicsItem *creatItem(CDrawToolEvent *event, ITERecordInfo *pInfo) override {return nullptr;}

    int  decideUpdate(CDrawToolEvent *event, ITERecordInfo *pInfo) override;

    void toolStart(CDrawToolEvent *event, ITERecordInfo *pInfo) override;

    void toolUpdate(CDrawToolEvent *event, ITERecordInfo *pInfo) override;

    void toolFinish(CDrawToolEvent *event, ITERecordInfo *pInfo) override;

    int  allowedMaxTouchPointCount() override {return 3;}

    bool returnToSelectTool(CDrawToolEvent *event, IDrawTool::ITERecordInfo *pInfo) override;

protected:
    QMap<int, QPainterPath> _resultImagePaths;
};


#endif // CCALLIGRAPHYPEN_H
