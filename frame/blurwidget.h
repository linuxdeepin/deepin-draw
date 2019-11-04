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
#ifndef BLURWIDGET_H
#define BLURWIDGET_H

#include <DWidget>
#include <DSlider>
#include <DLabel>


class CCheckButton;

DWIDGET_USE_NAMESPACE

class BlurWidget : public DWidget
{
    Q_OBJECT

public:
    BlurWidget(DWidget *parent = nullptr);
    ~BlurWidget();

    void updateBlurWidget();
    void changeButtonTheme();

private:
    void clearOtherSelections(CCheckButton *clickedButton);
    void initUI();
    void initConnection();

signals:
    void signalBlurAttributeChanged();

private:
    QList<CCheckButton *> m_actionButtons;
    CCheckButton *m_blurBtn;
    CCheckButton *m_masicBtn;
    DLabel *m_pLineWidthLabel;
    DSlider *m_pLineWidthSlider;
};

#endif // BLURWIDGET_H
