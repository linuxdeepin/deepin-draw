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

class CCutTool : public IDrawTool
{
public:
    CCutTool();
    virtual ~CCutTool() override;

protected:
    /**
     * @brief toolStart　工具开始事件
     * @param event      当次事件信息
     * @param pInfo      记录信息
     */
    virtual void toolStart(CDrawToolEvent *event, ITERecordInfo *pInfo) override;

    /**
     * @brief toolStart　判断工具活跃类型
     * @param event      当次事件信息
     * @param pInfo      记录信息
     */

    enum EOperateType { ENothing,
                        EDragMove,
                        EResizeMove
                      };
    virtual int decideUpdate(CDrawToolEvent *event, ITERecordInfo *pInfo) override;

    /**
     * @brief toolStart　 工具刷新事件
     * @param event       当次事件信息
     * @param pInfo       记录信息
     */
    virtual void toolUpdate(CDrawToolEvent *event, ITERecordInfo *pInfo) override;

    /**
     * @brief toolStart　工具结束事件
     * @param event      当次事件信息
     * @param pInfo      记录信息
     */
    virtual void toolFinish(CDrawToolEvent *event, ITERecordInfo *pInfo) override;

    /**
     * @brief toolStart　鼠标hover事件（处理高亮，鼠标样式变化等）
     * @param event      当次事件信息
     */
    virtual void mouseHoverEvent(IDrawTool::CDrawToolEvent *event) override;


public:
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
    QSizeF changeCutType(int, CDrawScene *scene);
    /**
     * @brief changeCutSize　改变裁剪尺寸
     */
    void changeCutSize(const CDrawScene *scene, const QSize &);

    /**
     * @brief doFinished　结束裁剪
     */
    void doFinished(bool accept);

    /**
     * @brief getCutStatus　获取裁剪状态（移动位置、大小改变都会返回true，否则返回false）
     */
    bool getCutStatus();

    /**
     * @brief getCutRect　获取裁剪区域矩形大小
     * @return
     */
    QRectF getCutRect(CDrawScene *scene);

    /**
     * @brief getCutType　获取裁剪区域裁剪类型
     * @return
     */
    int getCutType(CDrawScene *scene);

//    /**
//     * @brief getModifyFlag　获取是否修改标识
//     * @return
//     */
//    bool getModifyFlag() const;
    /**
     * @brief setModifyFlag　设置是否修改标识
     * @param flag
     */
//    void setModifyFlag(bool flag);

    CGraphicsCutItem *getCurCutItem();
    CGraphicsCutItem *getCutItem(CDrawScene *scene);

protected:
    bool returnToSelectTool(CDrawToolEvent *event, IDrawTool::ITERecordInfo *pInfo) override;

private:
    CGraphicsCutItem *m_pCutItem;
    CSizeHandleRect::EDirection m_dragHandle; //选中的方块方向

    bool m_bModify;

    // 用于保存剪裁图元避免多个场景有裁剪的时候其它场景不显示
    QMap<CDrawScene *, CGraphicsCutItem *> m_cutItems;
};

#endif // CCUTTOOL_H
