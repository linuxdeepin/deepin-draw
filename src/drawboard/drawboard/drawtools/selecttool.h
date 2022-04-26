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
#ifndef CSELECTTOOL_H
#define CSELECTTOOL_H
#include "drawfunctiontool.h"

#include <QCursor>
#include <QTime>
#include <QPainterPath>
#include <QElapsedTimer>

class PageItem;
class GroupItem;
class RectBaseItem;
class QUndoCommand;

class DRAWLIB_EXPORT SelectTool : public DrawFunctionTool
{
    Q_OBJECT
public:
    SelectTool(QObject *parent = nullptr);

    ~SelectTool() override;

    int toolType() const override;

    SAttrisList attributions() override;
    void  setAttributionVar(int attri, const QVariant &var, int phase, bool autoCmdStack) override;

    enum EOperateType { ENothingDo = 0,
                        ERectSelect,
                        EDragMove,
                        EResizeMove,
                        ERotateMove,
                        ECopyMove,
                        EDragSceneMove,
                        EOperateCount
                      };


    void funcStart(ToolSceneEvent *event) override;
    int  funcDecide(ToolSceneEvent *event) override;
    void funcUpdate(ToolSceneEvent *event, int decidedTp) override;
    void funHover(ToolSceneEvent *event) override;
    void funcFinished(ToolSceneEvent *event, int decidedTp) override;
    void doubleClickOnScene(ToolSceneEvent *event) override;

    void contextMenuEvent(ToolSceneEvent *event) override;
    void leaveSceneEvent(ToolSceneEvent *event) override;

    /**
     * @brief painter　绘制更多的内容（用于绘制框选矩形和高亮路径）
     * @param painter  绘制指针
     * @param rect     矩形大小
     * @param scene    场景指针
     */
    void drawMore(QPainter *painter, const QRectF &rect, PageScene *scene) override;

protected:
    void onStatusChanged(EStatus oldStatus, EStatus nowStatus) override;

    //bool returnToSelectTool(ToolSceneEvent *event) ;

    /**
     * @brief sendToolEventToItem 将工具事件发送给图元，事件将会根据各工具的类型生成对应的图元事件
     */
    void sendToolEventToItem(ToolSceneEvent *event,
                             EChangedPhase phase, int type) ;

    /**
     * @brief processItemsMove 处理图元的移动
     */
    void processItemsMove(ToolSceneEvent *event,
                          EChangedPhase phase);

    /**
     * @brief processItemsMove 处理图元的缩放调整大小
     */
    void processItemsScal(ToolSceneEvent *event,

                          EChangedPhase phase);

    /**
     * @brief processItemsMove 处理图元的旋转
     */
    void processItemsRot(ToolSceneEvent *event,

                         EChangedPhase phase);

    void processHightLight(ToolSceneEvent *event);

    void processCursor(ToolSceneEvent *event);

private:
    PRIVATECLASS(SelectTool)
};

#endif // CSELECTTOOL_H
