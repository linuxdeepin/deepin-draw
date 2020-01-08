/*
 * Copyright (C) 2019 ~ %YEAR% Deepin Technology Co., Ltd.
 *
 * Author:     Renran
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
#ifndef CPENWIDGET_H
#define CPENWIDGET_H

#include <DWidget>

#include "drawshape/globaldefine.h"

class CSideWidthWidget;
class BorderColorButton;
class CCheckButton;
class SeperatorLine;

DWIDGET_USE_NAMESPACE

class CPenWidget : public DWidget
{
    Q_OBJECT

public:
    CPenWidget(DWidget *parent = nullptr);
    ~CPenWidget();
    /**
     * @brief changeButtonTheme 根据主题改变按钮主题
     */
    void changeButtonTheme();

public slots:
    /**
     * @brief updatePenWidget　更新属性栏
     */
    void updatePenWidget();

signals:
    /**
     * @brief resetColorBtns　重置颜色按钮信号
     */
    void resetColorBtns();
    /**
     * @brief showColorPanel　显示调色板信号
     * @param drawstatus　颜色状态
     * @param pos　位置
     * @param visible　是否显示
     */
    void showColorPanel(DrawStatus drawstatus, QPoint pos, bool visible = true);
    /**
     * @brief signalPenAttributeChanged　属性变化信号
     */
    void signalPenAttributeChanged();

private:
    CSideWidthWidget *m_sideWidthWidget;
    BorderColorButton *m_strokeBtn;
    QList<CCheckButton *> m_actionButtons;
    CCheckButton *m_straightline;
    CCheckButton *m_arrowline;
    SeperatorLine *m_sep1Line;

private:
    /**
     * @brief initUI　初始化ＵＩ
     */
    void initConnection();
    /**
     * @brief initUI　初始化ＵＩ
     */
    void initUI();
    /**
     * @brief clearOtherSelections　清除其他按钮选中状态
     * @param clickedButton
     */
    void clearOtherSelections(CCheckButton *clickedButton);
};

#endif // CPENWIDGET_H
