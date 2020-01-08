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
#ifndef POLYGONALSTARATTRIBUTEWIDGET_H
#define POLYGONALSTARATTRIBUTEWIDGET_H

#include <DWidget>
#include <DSlider>
#include <DLineEdit>
#include "drawshape/globaldefine.h"

DWIDGET_USE_NAMESPACE

class BigColorButton;
class BorderColorButton;
class CSideWidthWidget;
class SeperatorLine;

class PolygonalStarAttributeWidget : public DWidget
{
    Q_OBJECT
public:
    PolygonalStarAttributeWidget(DWidget *parent = nullptr);
    ~PolygonalStarAttributeWidget();
    /**
     * @brief changeButtonTheme 根据主题改变按钮主题
     */
    void changeButtonTheme();

public slots:
    /**
     * @brief updatePolygonalStarWidget　更新属性栏
     */
    void updatePolygonalStarWidget();

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
     * @brief signalPolygonalStarAttributeChanged　属性变化信号
     */
    void signalPolygonalStarAttributeChanged();

private:
    BigColorButton *m_fillBtn;
    BorderColorButton *m_strokeBtn;
    CSideWidthWidget *m_sideWidthWidget;
    DSlider *m_anchorNumSlider; //锚点数滑块
    DLineEdit *m_anchorNumEdit;//锚点数编辑框
    DSlider *m_radiusNumSlider;//半径滑块
    DLineEdit *m_radiusNumEdit;//半径编辑框
    SeperatorLine *m_sepLine;
    QAction *m_addAction;
    QAction *m_reduceAction;


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

#endif // POLYGONALSTARATTRIBUTEWIDGET_H
