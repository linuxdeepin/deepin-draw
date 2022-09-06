// SPDX-FileCopyrightText: 2020 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef CPICTURETOOL_H
#define CPICTURETOOL_H

#include "idrawtool.h"
#include "cpictureitem.h"
#include "cdrawscene.h"
#include"widgets/progresslayout.h"
#include "frame/ccentralwidget.h"

#include <DFileDialog>


#include <DWidget>

DWIDGET_USE_NAMESPACE

class CPictureTool: public IDrawTool
{
    Q_OBJECT
public:
    explicit CPictureTool();
    ~CPictureTool();

    QAbstractButton *initToolButton() override;

    DrawAttribution::SAttrisList attributions() override;

    void  registerAttributionWidgets() override;

    void onStatusChanged(EStatus oldStatus, EStatus nowStatus) override;

    DECLAREPRIVATECLASS(CPictureTool)
};


#endif // CPICTURETOOL_H
