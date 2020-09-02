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
#ifndef LINEWIDGET_H
#define LINEWIDGET_H

#include <DWidget>
#include <DLabel>

#include "drawshape/globaldefine.h"

class CSideWidthWidget;
class BorderColorButton;
class SeperatorLine;
//class DMenuComboBox;

Q_DECLARE_METATYPE(ELineType);
DWIDGET_USE_NAMESPACE

class LineWidget : public DWidget
{
    Q_OBJECT
public:
    LineWidget(DWidget *parent = nullptr);
    ~LineWidget();
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
     * @brief updateLineWidget　更新属性栏
     */
    void updateLineWidget();
    /**
     * @brief slotSideWidthChoosed 描边粗细变化信号
     */
    void slotSideWidthChoosed(int width);
signals:
    /**
     * @brief resetColorBtns　重置颜色按钮
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
     * @brief signalLineAttributeChanged　线段属性变化信号
     */
    void signalLineAttributeChanged();

private:
    CSideWidthWidget *m_sideWidthWidget; // 线宽
    BorderColorButton *m_strokeBtn; // 线颜色
    SeperatorLine *m_sep1Line; // 竖线

//    DMenuComboBox *m_lineStartComboBox; // 线段起点样式
//    DMenuComboBox *m_lineEndComboBox; // 线段终点样式
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
    void initUI();
    /**
     * @brief initConnection　初始化连接
     */
    void initConnection();
};

#endif // LINEWIDGET_H
