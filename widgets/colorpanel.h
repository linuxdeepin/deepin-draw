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
     * @brief setDisableColor
     * @param disable
     */
    void setDisableColor(bool disable);

signals:
    /**
     * @brief colorButtonClicked
     * @param color
     */
    void colorButtonClicked(QColor color);

protected:
    void paintEvent(QPaintEvent *);

private:
    QColor m_color;
    bool m_disable;
};

class CAlphaControlWidget;
class PickColorWidget;

class ColorPanel : public DWidget
{
    Q_OBJECT
public:
    ColorPanel(DWidget *parent = nullptr);
    ~ColorPanel() override;
    /**
     * @brief updateColorPanel 更新调色板
     * @param status　状态
     */
    void updateColorPanel(DrawStatus status);

signals:
    /**
     * @brief updateHeight　更新调色板高度信号
     */
    void updateHeight();
    /**
     * @brief signalColorChanged　发送调色改变信号
     */
    void signalColorChanged();
    /**
     * @brief signalChangeFinished　发送改变结束信号
     */
    void signalChangeFinished();


public slots:
    /**
     * @brief slotPickedColorChanged　获取选择颜色
     */
    void slotPickedColorChanged(QColor);
    /**
     * @brief setConfigColor　设置配置颜色
     * @param color
     */
    void setConfigColor(QColor color);
    /**
     * @brief changeButtonTheme　改变按钮主题
     */
    void changeButtonTheme();

protected:
    /**
     * @brief mouseMoveEvent　鼠标移动事件
     */
    void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;


private:
    DLineEdit *m_colLineEdit;
    CIconButton *m_colorfulBtn;
    CAlphaControlWidget *m_alphaControlWidget;
    QList<QColor> m_colList;
    QList<ColorButton *> m_cButtonList;
    DrawStatus m_drawstatus;
    QButtonGroup *m_colorsButtonGroup;
    PickColorWidget *m_pickColWidget;
    bool m_expand;

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
     * @brief resetColorBtn　重置颜色按钮
     */
    void resetColorBtn();
    /**
     * @brief setConfigColorByColorName　通过颜色名字设置颜色
     * @param color　颜色
     */
    void setConfigColorByColorName(QColor color);
};

#endif // COLORPANEL_H
