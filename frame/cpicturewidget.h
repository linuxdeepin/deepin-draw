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
#ifndef CUTWIDGET_H
#define CUTWIDGET_H

#include <DWidget>

DWIDGET_USE_NAMESPACE

class CPictureWidget : public DWidget
{
    Q_OBJECT

public:
    CPictureWidget(DWidget *parent = nullptr);
    ~CPictureWidget();
    /**
     * @brief changeButtonTheme 根据主题改变按钮主题
     */
    void changeButtonTheme();
    /**
     * @brief setAdjustmentIsEnable 设置自适应按钮是否可用
     */
    void setAdjustmentIsEnable(bool isEnable);

    /**
     * @brief setRotationEnable 设置图片旋转按钮是否可用
     */
    void setRotationEnable(bool isEnable);

private:
    DPushButton *m_leftRotateBtn;
    DPushButton *m_rightRotateBtn;
    DPushButton *m_flipHBtn;
    DPushButton *m_flipVBtn;
    DPushButton *m_flipAdjustment;

private:
    /**
     * @brief initUI 初始化ＵＩ
     */
    void initUI();
    /**
     * @brief initConnection 初始化连接
     */
    void initConnection();

};
#endif // CUTWIDGET_H
