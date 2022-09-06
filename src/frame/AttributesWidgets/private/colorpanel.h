// SPDX-FileCopyrightText: 2020 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

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

class ColorButton : public QPushButton
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

    /**
     * @brief colorLineEditor 颜色编辑控件
     */
    DLineEdit *colorLineEditor();

    /**
     * @brief setExpandWidgetVisble 设置扩展界面是否显示
     */
    void setExpandWidgetVisble(bool visble);

signals:
    void colorChanged(const QColor &color, EChangedPhase phase);
public slots:
    /**
     * @brief setTheme　改变按钮主题
     */
    void setTheme(int theme);

protected:
    void showEvent(QShowEvent *event) override;

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
