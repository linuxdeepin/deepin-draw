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
#ifndef LINEWIDGET_H
#define LINEWIDGET_H

#include <DWidget>

#include "drawshape/globaldefine.h"


class CSideWidthWidget;
class BorderColorButton;
class SeperatorLine;

DWIDGET_USE_NAMESPACE

class LineWidget : public DWidget
{
    Q_OBJECT
public:
    LineWidget(DWidget *parent = nullptr);
    ~LineWidget();
    void changeButtonTheme();

public slots:
    void updateLineWidget();

signals:
    void resetColorBtns();
    void showColorPanel(DrawStatus drawstatus, QPoint pos, bool visible = true);
    void signalLineAttributeChanged();

private:
    CSideWidthWidget *m_sideWidthWidget;
    BorderColorButton *m_strokeBtn;
    SeperatorLine *m_sep1Line;

private:
    void initUI();
    void initConnection();
};

#endif // LINEWIDGET_H
