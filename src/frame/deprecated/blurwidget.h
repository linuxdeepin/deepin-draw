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
#ifndef BLURWIDGET_H
#define BLURWIDGET_H
#include "service/cmanagerattributeservice.h"

#include <DWidget>
#include <DSlider>
#include <DLabel>
#include <DToolButton>

class CSpinBox;

DWIDGET_USE_NAMESPACE

class BlurWidget : public DWidget
{
    Q_OBJECT

public:
    BlurWidget(DWidget *parent = nullptr);
    ~BlurWidget();
    /**
     * @brief updateBlurWidget 更新属性栏
     */
    void updateBlurWidget();
    /**
     * @brief changeButtonTheme 根据主题改变按钮主题
     */
    void changeButtonTheme();
    /**
     * @brief updateMultCommonShapWidget 更新多选时属性栏
     * @param propertys 要显示的控件和数值
     */
    void updateMultCommonShapWidget(QMap<EDrawProperty, QVariant> propertys, bool write2Cache = true);

private:
    /**
     * @brief initUI 初始化ＵＩ
     */
    void initUI();
    /**
     * @brief initConnection 初始化连接
     */
    void initConnection();

private:
    QList<DToolButton *> m_actionButtons;
    DToolButton *m_blurBtn;
    DToolButton *m_masicBtn;
    DLabel *m_pLineWidthLabel;
    CSpinBox *m_spinboxForLineWidth = nullptr;

    void updateIcon(bool masic);
};

#endif // BLURWIDGET_H
