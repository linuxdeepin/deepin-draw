/*
 * Copyright (C) 2019 ~ %YEAR% Deepin Technology Co., Ltd.
 *
 * Author:     Renran
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
#include "arrowrectangle.h"

#include <application.h>

ArrowRectangle::ArrowRectangle(ArrowDirection direction, DWidget *parent)
    : DArrowRectangle(direction, DArrowRectangle::FloatWidget, parent)
{
    connect(dApp, &Application::focusChanged, this, [ = ](DWidget * old, DWidget * now) {
        Q_UNUSED(old);
        if (now != this && !this->isAncestorOf(now)) {
            hide();
        }
    });
}

void ArrowRectangle::hideEvent(QHideEvent *e)
{
    Q_UNUSED(e);
    emit hideWindow();
}

ArrowRectangle::~ArrowRectangle()
{}
