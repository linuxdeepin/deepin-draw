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
#include <DLabel>

#include "drawshape/globaldefine.h"

class CSideWidthWidget;
class BorderColorButton;
class CCheckButton;
class SeperatorLine;
//class DMenuComboBox;

Q_DECLARE_METATYPE(EPenType);
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
    /**
     * @brief updateMultCommonShapWidget 更新多选时属性栏
     * @param propertys 要显示的控件和数值
     */
    void updateMultCommonShapWidget(QMap<EDrawProperty, QVariant> propertys, bool write2Cache = true);
public slots:
    /**
     * @brief updatePenWidget　更新属性栏
     */
    void updatePenWidget();
    /**
     * @brief slotSideWidthChoosed 描边粗细变化信号
     */
    void slotSideWidthChoosed(int width);
signals:
    /**
     * @brief signalPenAttributeChanged　属性变化信号
     */
    void signalPenAttributeChanged();

private:
    CSideWidthWidget *m_sideWidthWidget; // 线宽
    BorderColorButton *m_strokeBtn; // 线颜色
    SeperatorLine *m_sep1Line; // 竖线

    DComboBox *m_lineStartComboBox; // 线段起点样式
    DComboBox *m_lineEndComboBox; // 线段终点样式
    DLabel *m_startLabel;
    DLabel *m_endLabel;

    DLabel *m_maskLableStart;
    DLabel *m_maskLableEnd;
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
