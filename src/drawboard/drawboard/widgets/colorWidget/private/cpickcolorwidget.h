/*
 * Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
 *
 * Author:     Ji XiangLong <jixianglong@uniontech.com>
 *
 * Maintainer: WangYu <wangyu@uniontech.com>
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
#ifndef CPICKCOLORWIDGET_H
#define CPICKCOLORWIDGET_H

#include <QWidget>

#include "editlabel.h"
#include "colorlabel.h"
#include "colorslider.h"
#include "globaldefine.h"

#ifdef USE_DTK
DWIDGET_USE_NAMESPACE
#endif

class CIconButton;
class ColorPickerInterface;
class CAlphaControlWidget;

class DRAWLIB_EXPORT PickColorWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PickColorWidget(QWidget *parent);

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
    void setAlpha(int alpha, bool internalChanged);

    /**
     * @brief setTheme　更新按钮主题
     */
    void setTheme(int);
    void setColor(const QColor &c,
                  bool internalChanged,
                  EChangedPhase phase);

signals:
    /**
     * @brief colorChanged　获取颜色信号
     * @param color　颜色
     */
    void colorChanged(const QColor &color);
    void colorChanged(const QColor &color, EChangedPhase phase);

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
    void initUI();
    void initConnects();
private:
    DLineEdit *m_hexLineEdit;
    EditLabel *m_redEditLabel;
    EditLabel *m_greenEditLabel;
    EditLabel *m_blueEditLabel;

    CIconButton *m_picker;
    ColorLabel *m_colorLabel;
    ColorSlider *m_colorSlider;
    ColorPickerInterface *m_cp;
    CAlphaControlWidget *m_alphaControlWidget;
    QColor curColor;
};

#endif // CPICKCOLORWIDGET_H
