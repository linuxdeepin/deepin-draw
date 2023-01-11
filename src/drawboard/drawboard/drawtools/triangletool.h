// SPDX-FileCopyrightText: 2020 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef CTRIANGLETOOL_H
#define CTRIANGLETOOL_H
#include "drawitemtool.h"

class DRAWLIB_EXPORT TriangleTool : public DrawItemTool
{
    Q_OBJECT
public:
    TriangleTool(QObject *parent = nullptr);

    virtual ~TriangleTool() override;

    int toolType() const override;

    SAttrisList attributions() override;

protected:
    PageItem *drawItemStart(ToolSceneEvent *event) override;
    void      drawItemUpdate(ToolSceneEvent *event, PageItem *pItem) override;
};

#endif // CTRIANGLETOOL_H
