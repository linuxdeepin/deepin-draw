// SPDX-FileCopyrightText: 2020 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef CRECTTOOL_H
#define CRECTTOOL_H

#include "drawitemtool.h"

class DRAWLIB_EXPORT RectTool : public DrawItemTool
{
    Q_OBJECT
public:
    RectTool(QObject *parent = nullptr);
    virtual ~RectTool() override;

    int toolType() const override;

    SAttrisList attributions() override;
protected:
    PageItem *drawItemStart(ToolSceneEvent *event) override;
    void      drawItemUpdate(ToolSceneEvent *event, PageItem *pItem) override;
    void      drawItemFinish(ToolSceneEvent *event, PageItem *pItem) override;
};

#endif // CRECTTOOL_H
