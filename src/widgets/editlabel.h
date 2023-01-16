// SPDX-FileCopyrightText: 2020 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef EDITLABEL_H
#define EDITLABEL_H

#include <DLineEdit>
#include <DWidget>

DWIDGET_USE_NAMESPACE

class EditLabel : public DLineEdit
{
    Q_OBJECT
public:
    explicit EditLabel(DWidget *parent = nullptr);
    ~EditLabel();

};

#endif // EDITLABEL_H
