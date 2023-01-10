/*
 * Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
 *
 * Author:     TanLang <tanlang@uniontech.com>
 *
 * Maintainer: TanLang <tanlang@uniontech.com>
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
#ifndef CGROUPBUTTONWGT_H
#define CGROUPBUTTONWGT_H
#include <DToolButton>

#include "attributewidget.h"

DWIDGET_USE_NAMESPACE

class DrawBoard;
class PageScene;
class GroupButtonWidget : public AttributeWgt
{
    Q_OBJECT
public:
    explicit GroupButtonWidget(DrawBoard *drawBoard, QWidget *parent = nullptr);

    void setGroupFlag(bool canGroup, bool canUngroup);
private:
    void updateButtonStatus();

signals:
    void buttonClicked(bool doGroup, bool doUngroup);

private:
    DToolButton *groupButton;
    DToolButton *unGroupButton;
    QLabel *m_titleLabel;
    DrawBoard   *m_drawBoard = nullptr;
    PageScene   *m_currentScene = nullptr;
};

#endif // CGROUPBUTTONWGT_H
