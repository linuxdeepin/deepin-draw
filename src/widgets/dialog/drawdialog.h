// SPDX-FileCopyrightText: 2020 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef DRAWDIALOG_H
#define DRAWDIALOG_H

#include "dialog.h"


class DrawDialog : public DDialog
{
    Q_OBJECT
public:
    explicit DrawDialog(DWidget *parent = nullptr);


signals:
    void signalSaveToDDF();
    void singalDoNotSaveToDDF();


protected:
    void keyPressEvent(QKeyEvent *e) override;
    void showEvent(QShowEvent *event) override;

};

#endif // DRAWDIALOG_H
