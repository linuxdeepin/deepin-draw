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
#ifndef CALPHACONTROLWIDGET_H
#define CALPHACONTROLWIDGET_H

#include <DWidget>
#include <DSlider>
#include <DLabel>

DWIDGET_USE_NAMESPACE

class CAlphaControlWidget : public DWidget
{
    Q_OBJECT
public:
    explicit CAlphaControlWidget(DWidget *parent = nullptr);
    void updateAlphaControlWidget(int alpha);
signals:
    void signalAlphaChanged(int);
    void signalFinishChanged();

private:
    DSlider *m_alphaSlider;
    DLabel *m_alphaLabel;
    bool m_isUserOperation;

private:
    void initUI();
    void initConnection();
};

#endif // CALPHACONTROLWIDGET_H
