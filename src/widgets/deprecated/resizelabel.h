/*
 * Copyright (C) 2019 ~ %YEAR% Deepin Technology Co., Ltd.
 *
 * Author:     WangXin
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
#ifndef RESIZELABEL_H
#define RESIZELABEL_H

#include <DLabel>
#include <DWidget>

#include <QPaintEvent>

#include "utils/shapesutils.h"

DWIDGET_USE_NAMESPACE

class ResizeLabel : public DLabel
{
    Q_OBJECT
public:
    explicit ResizeLabel(DWidget *parent = 0);
    ~ResizeLabel();

public slots:
    void paintResizeLabel(bool drawing, FourPoints fpoints);

protected:
    void paintEvent(QPaintEvent *e);

private:
    bool m_drawArtboard;
    FourPoints m_artboardMPoints;

};

#endif // RESIZELABEL_H
