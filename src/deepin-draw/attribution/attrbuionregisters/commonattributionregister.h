/*
 * Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
 *
 * Author:     TanLang <tanlang@uniontech.com>
 *
 * Maintainer: TanLang <tanlang@uniontech.com>
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
#ifndef ATTRIBUTIONREGISTER_H
#define ATTRIBUTIONREGISTER_H

#include <QStyleOptionMenuItem>
#include <QModelIndex>
#include "iattributionregister.h"

class GroupButtonWidget;
class RotateAttriWidget;
class OrderWidget;

class DrawBoard;
class ColorStyleWidget;
class SideWidthWidget;
class RectRadiusStyleWidget;
class CSpinBox;
class QComboBox;
class QToolButton;

class SliderSpinBoxWidget;
class ComboBoxSettingWgt;
class CheckBoxSettingWgt;
class HBoxLayoutWidget;
class CutAttributionWidget;

class CommonAttributionRegister : public IAttributionRegister
{
    Q_OBJECT
public:
    using IAttributionRegister::IAttributionRegister;
    virtual void registe() override;
    QList<QWidget *> getStyleAttriWidgets();
private:
    void registeGroupAttri();
    void resgisteRotateAttri();
    void registeOrderAttri();
    // 图片自适应工具
    void registeAdjustImageAttri();
    void registeBaseStyleAttrri();

    // 注册星形锚点工具
    void registeStarAnchorAttri();
    // 组成星形半径工具
    void registeStarInnerOuterRadioAttri();
    // 注册多边形侧边数工具
    void registePolygonSidesAttri();

    void registeLineArrowAttri();

    void registePenAttri();
    //注册橡皮擦工具
    void registeEraserAttri();
    //注册模糊工具
    void registeBlurAttri();
    //注册裁剪工具
    void registeCutAttri();
private:
    //更新成组解组按钮
    void updateGroupStatus();
private:
    bool m_isInit = false;      // 判断是否已调用函数

    GroupButtonWidget   *m_groupWidget = nullptr;
    RotateAttriWidget   *m_rotateAttri = nullptr;
    OrderWidget         *m_orderAttri = nullptr;
    ColorStyleWidget    *m_fillBrushStyle = nullptr;
    ColorStyleWidget    *m_borderPenStyle = nullptr;
    CSpinBox            *m_penWidth = nullptr;
    RectRadiusStyleWidget *m_rectRadius = nullptr;

    SliderSpinBoxWidget *m_starAnchorAttri = nullptr;           // 星形图元锚点属性
    SliderSpinBoxWidget *m_starRadioAttri = nullptr;            // 星形图元半径属性
    SliderSpinBoxWidget *m_polygonSidesAttri = nullptr;         // 多边形图元侧边数属性

    HBoxLayoutWidget *m_streakStyle = nullptr;
    QComboBox *m_comboxstart = nullptr;
    QComboBox *m_comboxend = nullptr;

    SliderSpinBoxWidget *m_eraserAttri = nullptr;         // 橡皮檫宽度属性

    QToolButton *m_blurEffect = nullptr;           //模糊
    QToolButton *m_masicoEffect = nullptr;           //高斯模糊
    SliderSpinBoxWidget *m_blurAttri = nullptr;         // 模糊宽度属性

    CutAttributionWidget *m_cutAttri = nullptr;     //裁剪属性

    ComboBoxSettingWgt  *m_penStyle = nullptr;
    SliderSpinBoxWidget *m_sliderPenWidth = nullptr;
    CheckBoxSettingWgt  *m_enablePenStyle = nullptr;
    CheckBoxSettingWgt  *m_enableBrushStyle = nullptr;
};

#endif // ATTRIBUTIONREGISTER_H
