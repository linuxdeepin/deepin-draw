/*
 * Copyright (C) 2019 ~ %YEAR% Deepin Technology Co., Ltd.
 *
 * Author:     WangXing
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
#ifndef SEPERATORLINE_H
#define SEPERATORLINE_H

#include <DLabel>
#include <DApplicationHelper>
#include <QDebug>
#include <drawshape/cdrawparamsigleton.h>
DWIDGET_USE_NAMESPACE
class SeperatorLine : public DLabel
{
    Q_OBJECT
public:
    SeperatorLine(DWidget *parent = 0);

    ~SeperatorLine();

    void updateTheme();

};
#endif // SEPERATORLINE_H
