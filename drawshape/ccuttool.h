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
#ifndef CCUTTOOL_H
#define CCUTTOOL_H

#include "idrawtool.h"
#include "cgraphicscutitem.h"
#include "csizehandlerect.h"
#include "cbuttonrect.h"


class CCutTool : public IDrawTool
{
public:
    CCutTool();
    virtual ~CCutTool() Q_DECL_OVERRIDE;

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
    /**
     * @brief createCutItem 创建裁剪图元
     * @param scene　场景句柄
     */
    void createCutItem(CDrawScene *scene);
    /**
     * @brief deleteCutItem　删除裁剪图元
     * @param scene　场景句柄
     */
    void deleteCutItem(CDrawScene *scene);
    /**
     * @brief changeCutType　改变裁剪类型
     * @param scene　场景句柄
     */
    void changeCutType(int, CDrawScene *scene);
    /**
     * @brief changeCutSize　改变裁剪尺寸
     */
    void changeCutSize(const CDrawScene *scene, const QSize &);
    /**
     * @brief getCutRect　获取裁剪区域矩形大小
     * @return
     */
    QRectF getCutRect(CDrawScene *scene);
    /**
     * @brief getModifyFlag　获取是否修改标识
     * @return
     */
    bool getModifyFlag() const;
    /**
     * @brief setModifyFlag　设置是否修改标识
     * @param flag
     */
    void setModifyFlag(bool flag);

private:
    CGraphicsCutItem *m_pCutItem;
    CSizeHandleRect::EDirection m_dragHandle; //选中的方块方向
    CButtonRect::EButtonType m_buttonType;

    bool m_bModify;

    // 用于保存剪裁图元避免多个场景有裁剪的时候其它场景不显示
    QMap<CDrawScene *, CGraphicsCutItem *> m_cutItems;
};

#endif // CCUTTOOL_H
