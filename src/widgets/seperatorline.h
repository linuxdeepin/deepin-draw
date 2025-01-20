// SPDX-FileCopyrightText: 2020 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef SEPERATORLINE_H
#define SEPERATORLINE_H

#include <DLabel>
#include <DWidget>

#include <DGuiApplicationHelper>
#include <QDebug>


DWIDGET_USE_NAMESPACE
class SeperatorLine : public DLabel
{
    Q_OBJECT
public:
    explicit SeperatorLine(DWidget *parent = nullptr);

    ~SeperatorLine();

    void updateTheme();

};
#endif // SEPERATORLINE_H
