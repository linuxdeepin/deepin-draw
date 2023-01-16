// SPDX-FileCopyrightText: 2020 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

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
