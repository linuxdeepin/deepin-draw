// SPDX-FileCopyrightText: 2020 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef CSHAPEMIMEDATA_H
#define CSHAPEMIMEDATA_H

#include "sitemdata.h"

#include <QGraphicsItem>
#include <QMimeData>

class CGraphicsItem;

class CShapeMimeData : public QMimeData
{
    Q_OBJECT
public:

    explicit CShapeMimeData(const CGroupBzItemsTreeInfo &itemsTreeInfo);
    ~CShapeMimeData();

    bool hasFormat(const QString &mimetype) const;

    CGroupBzItemsTreeInfo itemsTreeInfo();

private:

    CGroupBzItemsTreeInfo treeInfo = CGroupBzItemsTreeInfo();
};

#endif // CSHAPEMIMEDATA_H
