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
#ifndef COLORPANEL_H
#define COLORPANEL_H

#include <DPushButton>
#include <QWidget>
#include <QColor>
#include <QPaintEvent>
#include <QApplication>

#include "editlabel.h"
#include "drawshape/globaldefine.h"

class CIconButton;

DWIDGET_USE_NAMESPACE

class ColorButton : public DPushButton
{
    Q_OBJECT
public:
    ColorButton(const QColor &color, DWidget *parent = nullptr);
    ~ColorButton();

    /**
     * @brief color  按钮代表的颜色
     * @param disable
     */
    QColor color();

protected:
    void paintEvent(QPaintEvent *);

private:
    const QColor m_color;
};

class CAlphaControlWidget;
class PickColorWidget;
class CColorPickWidget;

class ColorPanel : public DWidget
{
    Q_OBJECT
public:
    explicit ColorPanel(DWidget *parent = nullptr);

    ~ColorPanel() override;

    CColorPickWidget *parentColorWidget();

    /**
     * @brief updateColor 设置颜色
     */
    void setColor(const QColor &c, bool internalChanged = true, EChangedPhase phase = EChanged);

    /**
     * @brief color 当前颜色
     */
    QColor color();

signals:
    void colorChanged(const QColor &color, EChangedPhase phase);
public slots:
    /**
     * @brief setTheme　改变按钮主题
     */
    void setTheme(int theme);

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
     * @brief updateColor 刷新颜色
     */
    void updateColor(const QColor &previewColor = QColor());


    /**
     * @brief updateExpendArea 刷新扩展区域
     */
    void updateExpendArea();

private:
    /* pick widget 十六进制颜色编辑 */
    DLineEdit *m_colLineEdit;

    /* pick widget 开关按钮 */
    CIconButton *m_colorfulBtn;

    /* 控制透明度的值 */
    CAlphaControlWidget *m_alphaControlWidget;

    /* pick color 界面*/
    PickColorWidget *m_pickColWidget;

    /* 默认的颜色组 */
    QList<ColorButton *> m_cButtonList;
    QList<QColor> m_colList;
    QButtonGroup *m_colorsButtonGroup;

    /* 当前 pick widget 是否是展开的标记 */
    static bool s_expand;

    /* 当前 颜色 */
    QColor curColor;
};

#endif // COLORPANEL_H
