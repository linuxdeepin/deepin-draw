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
#include "iattributionregister.h"

class GroupButtonWidget;
class RotateAttriWidget;
class OrderWidget;

class ColorStyleWidget;
class RectRadiusStyleWidget;
class CSpinBox;

class SliderSpinBoxWidget;
class ComboBoxSettingWgt;
class CheckBoxSettingWgt;

class GroupAttriRegister : public IAttributionRegister
{
    Q_OBJECT
public:
    using IAttributionRegister::IAttributionRegister;
    virtual void registe() override;
private:
    void updateGroupStatus();
private:
    GroupButtonWidget   *m_groupWidget = nullptr;
};

class RotateAttriRegister : public IAttributionRegister
{
    Q_OBJECT
public:
    using IAttributionRegister::IAttributionRegister;
    virtual void registe() override;
private:
    RotateAttriWidget   *m_rotateAttri = nullptr;
};

class OrderAttriRegister : public IAttributionRegister
{
    Q_OBJECT
public:
    using IAttributionRegister::IAttributionRegister;
    virtual void registe() override;
private:
    OrderWidget *m_orderAttri = nullptr;
};

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
    void registeBaseStyleAttrri();
    void registeOrderAttri();
private:
    bool m_isInit = false;      // 判断是否已调用函数

    ColorStyleWidget    *m_fillBrushStyle = nullptr;
    ColorStyleWidget    *m_borderPenStyle = nullptr;
    CSpinBox            *m_penWidth = nullptr;

    CheckBoxSettingWgt  *m_enablePenStyle = nullptr;
    CheckBoxSettingWgt  *m_enableBrushStyle = nullptr;
};

#endif // ATTRIBUTIONREGISTER_H
