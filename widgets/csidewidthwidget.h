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
#ifndef CSIDEWIDTHWIDGET_H
#define CSIDEWIDTHWIDGET_H

#include <DFrame>
#include <QHBoxLayout>
#include <DWidget>

DWIDGET_USE_NAMESPACE


class CCheckButton;

class CSideWidthWidget : public DFrame
{
public:
    enum CLineWidth {
        Finer = 2,
        Fine = 4,
        Medium = 6,
        Bold = 8
    };

    Q_OBJECT
public:
    explicit CSideWidthWidget(DWidget *parent = nullptr);

signals:
    void signalSideWidthChange();
public:
    void updateSideWidth();
    void changeButtonTheme();

private:
    CCheckButton *m_finerButton;
    CCheckButton *m_fineButton;
    CCheckButton *m_mediumButton;
    CCheckButton *m_boldButton;

    QHBoxLayout *m_layout;
    QMap<CCheckButton *, CLineWidth> m_buttonMap;

private:
    void initUI();
    void initConnection();
    void clearOtherSelections(CCheckButton *clickedButton);
};

#endif // CSIDEWIDTHWIDGET_H
