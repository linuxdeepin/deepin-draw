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
#include <QObject>

class GroupButtonWidget;
class RotateAttriWidget;
class OrderWidget;

class DrawBoard;
class ColorStyleWidget;
class SideWidthWidget;
class RectRadiusStyleWidget;
class CSpinBox;

class AttributionRegister : public QObject
{
    Q_OBJECT
public:
    explicit AttributionRegister(DrawBoard *d);
    void registe();
    void initConnect();
    void registeGroupAttri();
    void resgisteRotateAttri();
    void registeOrderAttri();
    //图片自适应
    void registeAdjustImageAttri();
    void registeStyleAttri();
    void registeBaseStyleAttrri();

private:
    DrawBoard *m_drawBoard;
    GroupButtonWidget *m_groupWidget = nullptr;
    RotateAttriWidget *m_rotateAttri = nullptr;
    OrderWidget *m_orderAttri = nullptr;
    ColorStyleWidget *m_fillStyle = nullptr;
    ColorStyleWidget *m_borderStyle = nullptr;
    CSpinBox *m_penWidth = nullptr;
    RectRadiusStyleWidget *m_rectRadius = nullptr;
};

#endif // ATTRIBUTIONREGISTER_H
