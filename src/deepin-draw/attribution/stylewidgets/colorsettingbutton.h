/*
 * Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
 *
 * Author:     Tan Lang <tanlang@uniontech.com>
 *
 * Maintainer: Tan Lang <tanlang@uniontech.com>
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
#ifndef COLORSETTINGBUTTON_H
#define COLORSETTINGBUTTON_H
#include "attributewidget.h"

class ColorPickWidget;
class CColorSettingButton: public ColorSettingButton
{
    Q_OBJECT
public:
    using ColorSettingButton::ColorSettingButton;
    ColorPickWidget *colorPick();
protected:
    void  mousePressEvent(QMouseEvent *event) override;
private:
    bool        _connectedColorPicker = false;

};


#endif // COLORSETTINGBUTTON_H
