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
#ifndef POLYGONATTRIBUTEWIDGET_H
#define POLYGONATTRIBUTEWIDGET_H

#include <DWidget>
#include <DSpinBox>
#include <DLabel>

#include "drawshape/globaldefine.h"

DWIDGET_USE_NAMESPACE

class BigColorButton;
class BorderColorButton;
class CSideWidthWidget;
class SeperatorLine;
class CSpinBox;

class PolygonAttributeWidget : public DWidget
{
    Q_OBJECT
public:
    PolygonAttributeWidget(DWidget *parent = nullptr);
    ~PolygonAttributeWidget();
    /**
     * @brief changeButtonTheme 根据主题改变按钮主题
     */
    void changeButtonTheme();
    /**
     * @brief updateMultCommonShapWidget 更新多选时属性栏
     * @param propertys 要显示的控件和数值
     */
    void updateMultCommonShapWidget(QMap<EDrawProperty, QVariant> propertys);
public slots:
    /**
     * @brief updatePolygonWidget　更新属性栏
     */
    void updatePolygonWidget();
    /**
     * @brief slotRadiusvalueChanged　侧边数值改变处理
     */
    void slotSideValueChanged(int value);
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
     * @brief signalPolygonAttributeChanged　属性变化信号
     */
    void signalPolygonAttributeChanged();
    /**
     * @brief signalSideValueIsfocus　边数变化信号
     */
    void signalSideValueIsfocus(bool);

private:
    BigColorButton *m_fillBtn;
    BorderColorButton *m_strokeBtn;
    CSideWidthWidget *m_sideWidthWidget;
    CSpinBox *m_sideNumSlider;
    SeperatorLine *m_sepLine;
//    DLabel *m_lwLabel;
    DLabel *m_sideNumLabel;

private:
    /**
     * @brief initUI　初始化ＵＩ
     */
    void initUI();
    /**
     * @brief initConnection　初始化连接
     */
    void initConnection();
    /**
     * @brief getBtnPosition 获取按钮位置
     * @param btn　按钮
     * @return
     */
    QPoint getBtnPosition(const DPushButton *btn);
};


#endif // POLYGONATTRIBUTEWIDGET_H
