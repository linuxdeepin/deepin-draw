#ifndef CMASICOTOOL_H
#define CMASICOTOOL_H
#include "idrawtool.h"

class CGraphicsMasicoItem;
class CMasicoTool : public IDrawTool
{
public:
    CMasicoTool();
    ~CMasicoTool() Q_DECL_OVERRIDE;

public:
    /**
     * @brief mousePressEvent　鼠标点击事件
     * @param event　场景事件
     * @param scene　场景句柄
     */
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene) Q_DECL_OVERRIDE;
    /**
     * @brief mouseMoveEvent 鼠标移动事件
     * @param event 场景事件
     * @param scene 场景句柄
     */
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene) Q_DECL_OVERRIDE;
    /**
     * @brief mouseReleaseEvent　鼠标弹起事件
     * @param event 场景事件
     * @param scene 场景句柄
     */
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene)Q_DECL_OVERRIDE;

private:
    CGraphicsMasicoItem *m_pBlurItem;
};

#endif // CMASICOTOOL_H
