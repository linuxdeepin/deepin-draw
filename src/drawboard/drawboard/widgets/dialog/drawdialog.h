/*
 * Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
 *
 * Author:     Ji XiangLong <jixianglong@uniontech.com>
 *
 * Maintainer: WangYu <wangyu@uniontech.com>
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
#ifndef DRAWDIALOG_H
#define DRAWDIALOG_H

#include "config.h"

#ifdef USE_DTK
#include <DDialog>
DWIDGET_USE_NAMESPACE
class DrawDialog : public DDialog
#else
#include "dialog.h"
class DrawDialog : public MessageDlg
#endif
{
    Q_OBJECT
public:
    explicit DrawDialog(QWidget *parent = nullptr);


signals:
    void signalSaveToDDF();
    void singalDoNotSaveToDDF();


protected:
    void keyPressEvent(QKeyEvent *e) override;
    void showEvent(QShowEvent *event) override;

};

#endif // DRAWDIALOG_H
