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
#ifndef TEXTWIDGET_H
#define TEXTWIDGET_H

#include <DWidget>
#include <DSlider>
#include <DLineEdit>
#include <DFontComboBox>

#include "drawshape/globaldefine.h"

DWIDGET_USE_NAMESPACE

class TextColorButton;
class SeperatorLine;

class TextWidget : public DWidget
{
    Q_OBJECT
public:
    TextWidget(DWidget *parent = nullptr);
    ~TextWidget();

public slots:
    void updateTextWidget();
    void updateTextColor();
    void updateTheme();

signals:
    void showColorPanel(DrawStatus drawstatus, QPoint pos, bool visible = true);
    void resetColorBtns();
    void signalTextAttributeChanged();
    void signalTextFontFamilyChanged();
    void signalTextFontSizeChanged();

private:
    TextColorButton *m_fillBtn;
    DSlider *m_fontSizeSlider;
    DLineEdit *m_fontSizeEdit;
    DFontComboBox *m_fontComBox;
    SeperatorLine *m_textSeperatorLine;


private:
    void initUI();
    void initConnection();
};

#endif // TEXTWIDGET_H
