// SPDX-FileCopyrightText: 2020 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

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
    explicit PickColorWidget(DWidget *parent);

    ~PickColorWidget();

    /**
     * @brief color 当前颜色
     */
    QColor color();

    /**
     * @brief setColor 设置颜色
     * @param c　颜色
     * @param internalChanged　由内变化
     */
    void setColor(const QColor &c, bool internalChanged = true);

    /**
     * @brief setTheme　更新按钮主题
     */
    void setTheme(int);

signals:
    /**
     * @brief colorChanged　获取颜色信号
     * @param color　颜色
     */
    void colorChanged(const QColor &color);

    /**
     * @brief previewedColorChanged　颜色预览信号
     * @param color　颜色
     */
    void previewedColorChanged(const QColor &color);

private:
    /**
     * @brief updateColor　更新颜色
     */
    void updateColor(const QColor &color = QColor());

private:
    EditLabel *m_redEditLabel;
    EditLabel *m_greenEditLabel;
    EditLabel *m_blueEditLabel;

    CIconButton *m_picker;
    ColorLabel *m_colorLabel;
    ColorSlider *m_colorSlider;
    ColorPickerInterface *m_cp;

    QColor curColor;
};

#endif // PICKCOLORWIDGET_H
