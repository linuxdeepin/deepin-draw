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
#ifndef CPENWIDGET_H
#define CPENWIDGET_H

#include <DWidget>

#include "drawshape/globaldefine.h"

class CSideWidthWidget;
class BorderColorButton;
class CCheckButton;

DWIDGET_USE_NAMESPACE

class CPenWidget : public DWidget
{
    Q_OBJECT

public:
    CPenWidget(DWidget *parent = nullptr);
    ~CPenWidget();

    void changeButtonTheme();

public slots:
    void updatePenWidget();

signals:
    void resetColorBtns();
    void showColorPanel(DrawStatus drawstatus, QPoint pos, bool visible = true);
    void signalPenAttributeChanged();

private:
    CSideWidthWidget *m_sideWidthWidget;
    BorderColorButton *m_strokeBtn;
    QList<CCheckButton *> m_actionButtons;
    CCheckButton *m_straightline;
    CCheckButton *m_arrowline;

private:
    void initConnection();
    void initUI();
    void clearOtherSelections(CCheckButton *clickedButton);
};

#endif // CPENWIDGET_H
