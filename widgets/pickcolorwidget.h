/*
 * Copyright (C) 2019 ~ %YEAR% Deepin Technology Co., Ltd.
 *
 * Author:     RenRan
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
#ifndef PICKCOLORWIDGET_H
#define PICKCOLORWIDGET_H

#include <DWidget>

#include "editlabel.h"
#include "colorlabel.h"
#include "colorslider.h"

DWIDGET_USE_NAMESPACE

class CIconButton;
class ColorPickerInterface;

class PickColorWidget : public DWidget
{
    Q_OBJECT
public:
    PickColorWidget(DWidget *parent);
    ~PickColorWidget();
    /**
     * @brief setRgbValue 设置ＲＧＢ值
     * @param color　颜色
     * @param isPicked　是否获取颜色
     */
    void setRgbValue(QColor color, bool isPicked = false);
    /**
     * @brief updateColor　更新颜色
     */
    void updateColor();
    /**
     * @brief setPickedColor　设置是否获取颜色
     * @param picked　是否获取颜色
     */
    void setPickedColor(bool picked);
    /**
     * @brief updateButtonTheme　更新按钮主题
     */
    void updateButtonTheme(int);

signals:
    /**
     * @brief pickedColor　获取颜色信号
     * @param color　颜色
     */
    void pickedColor(QColor color);
    /**
     * @brief signalPreSetColorName　颜色预览信号
     * @param color　颜色
     */
    void signalPreSetColorName(QColor color);


private:
    EditLabel *m_redEditLabel;
    EditLabel *m_greenEditLabel;
    EditLabel *m_blueEditLabel;
    CIconButton *m_picker;
    ColorLabel *m_colorLabel;
    ColorSlider *m_colorSlider;
    ColorPickerInterface *m_cp;

private:
    void preSetRgbValue(QColor color);
};

#endif // PICKCOLORWIDGET_H
