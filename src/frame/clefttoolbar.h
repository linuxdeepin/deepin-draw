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
#ifndef RIGHTTOOLBAR_H
#define RIGHTTOOLBAR_H

#include "globaldefine.h"
#include "cgraphicsitem.h"
#include "cdrawtoolfactory.h"

#include <DFrame>
#include <DWidget>
#include <DPushButton>
#include <DToolButton>

class QVBoxLayout;
class PushButton;

class DrawBoard;


DWIDGET_USE_NAMESPACE

class DrawToolManager : public DFrame
{
    Q_OBJECT
public:
    explicit DrawToolManager(DrawBoard *parent = nullptr);
    ~DrawToolManager();

    bool setCurrentTool(int tool, bool force = false);
    bool setCurrentTool(IDrawTool *tool, bool force = false);

    int  currentTool()const;
    IDrawTool *tool(int tool)const;

    DrawBoard *drawBoard() const;


    void installTool(IDrawTool *tool);
    //void removeTool(IDrawTool *tool);


    void registerAllTools();

signals:
    void currentToolChanged(int oldTool, int nowTool);

protected:
    void mouseMoveEvent(QMouseEvent *event) override;
    void enterEvent(QEvent *event) override;                      //进入QWidget瞬间事件
    void paintEvent(QPaintEvent *event) override;

private:
    /**
     * @brief initUI　初始化ＵＩ
     */
    void initUI();

    /**
     * @brief initDrawTools　初始化画图工具
     */
    void initDrawTools();

private:
    QVBoxLayout  *m_layout;
    QButtonGroup *toolButtonGroup = nullptr;

    IDrawTool *_currentTool = nullptr;
    CDrawToolFactory::CDrawToolsMap _tools;
    DrawBoard   *m_drawBoard = nullptr;

};

#endif // RIGHTTOOLBAR_H
