// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef TESTGRAPHICSITEM_H
#define TESTGRAPHICSITEM_H

#include <QGraphicsItem>
#include <QRectF>

#include "drawshape/drawItems/bzItems/cgraphicsitem.h"

// CGraphicsItem is abstract (pure virtual rect()), so we need a concrete
// subclass for testing the base-class methods directly.
class TestGraphicsItem : public CGraphicsItem
{
public:
    explicit TestGraphicsItem(QGraphicsItem *parent = nullptr)
        : CGraphicsItem(parent) {}
    QRectF m_testRect;
    QRectF rect() const override { return m_testRect; }
};

#endif // TESTGRAPHICSITEM_H
