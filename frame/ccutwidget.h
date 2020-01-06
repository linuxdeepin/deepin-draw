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
#ifndef AILORINGWIDGET_H
#define AILORINGWIDGET_H

#include "widgets/seperatorline.h"
#include <DWidget>
#include <DLineEdit>
#include <QAction>

DWIDGET_USE_NAMESPACE

class CClickButton;
class CCutWidget : public DWidget
{
    Q_OBJECT
public:


public:
    explicit CCutWidget(DWidget *parent = nullptr);
    ~CCutWidget();

    void updateCutSize();
    void clearAllChecked();
    void changeButtonTheme();
    void updateButtonStatus();


signals:
    void signalCutAttributeChanged();
    void signalCutLineEditIsfocus(bool);

public slots:

private:
    DPushButton *m_scaleBtn1_1;
    DPushButton *m_scaleBtn2_3;
    DPushButton *m_scaleBtn8_5;
    DPushButton *m_scaleBtn16_9;

    DPushButton *m_freeBtn;
    DPushButton *m_originalBtn;

    DLineEdit *m_widthEdit;
    DLineEdit *m_heightEdit;

    CClickButton *m_cutBtn;
    CClickButton *m_cancelBtn;
    SeperatorLine *m_sepLine;

    QAction *m_SizeAddAction;
    QAction *m_SizeReduceAction;

private:
    void initUI();
    void initConnection();
    void activeFreeMode();
};

#endif // AILORINGWIDGET_H
