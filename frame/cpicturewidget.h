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
#ifndef CUTWIDGET_H
#define CUTWIDGET_H

#include <DWidget>

class CClickButton;

DWIDGET_USE_NAMESPACE

class CPictureWidget : public DWidget
{
public:
    enum EButtonType {
        LeftRotate,     //左旋转
        RightRotate,    //右旋转
        FlipHorizontal, //水平翻转
        FlipVertical    //垂直翻转
    };

    Q_OBJECT
public:
    CPictureWidget(DWidget *parent = nullptr);
    ~CPictureWidget();

    void changeButtonTheme();

signals:
    void signalBtnClick(int);

private:
    CClickButton *m_leftRotateBtn;
    CClickButton *m_rightRotateBtn;
    CClickButton *m_flipHBtn;
    CClickButton *m_flipVBtn;

private:
    void initUI();
    void initConnection();

};
#endif // CUTWIDGET_H
