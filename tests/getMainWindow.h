/*
* Copyright (C) 2019 ~ 2020 UnionTech Software Technology Co.,Ltd
*
* Author:     zhanghao<zhanghao@uniontech.com>
* Maintainer: zhanghao<zhanghao@uniontech.com>
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* any later version.
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef MIANWINDOW_H
#define MIANWINDOW_H

#include "mainwindow.h"
#include "ccentralwidget.h"
#include "cgraphicsview.h"
#include "application.h"

static MainWindow *getMainWindow()
{
    static MainWindow *temp_window = nullptr;
    if (temp_window == nullptr) {
        QStringList li;
        temp_window = new MainWindow(li);
        dApp->setActivationWindow(temp_window, true);
        temp_window->show();
    }
    return temp_window;
}

#endif // MIANWINDOW_H
