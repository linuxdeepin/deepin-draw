// SPDX-FileCopyrightText: 2020 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef CLINETOOL_H
#define CLINETOOL_H
#include "drawitemtool.h"

class DRAWLIB_EXPORT LineTool : public DrawItemTool
{
    Q_OBJECT
public:
    LineTool(QObject *parent = nullptr);
    virtual ~LineTool() override;
    int toolType() const override;

    SAttrisList attributions() override;

    PageItem *drawItemStart(ToolSceneEvent *event) override;
    void      drawItemUpdate(ToolSceneEvent *event, PageItem *pItem) override;
    void      drawItemFinish(ToolSceneEvent *event, PageItem *pItem) override;
};

#endif // CLINETOOL_H
