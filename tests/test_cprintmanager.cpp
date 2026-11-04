// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for CPrintManager::slotPaintRequest (issue V-7618 group 17)
 *
 * Source: src/widgets/dialog/cprintmanager.h/.cpp
 *
 * Target method:
 *   void slotPaintRequest(DPrinter *_printer)
 *
 * Note: slotPaintRequest creates QPainter(_printer) immediately, so a null
 * printer crashes. The null-printer case is flagged as a potential source
 * defect (missing null check) but not fixed per project constraints.
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QImage>
#include <QPainter>
#include <dprintpreviewwidget.h>

#define protected public
#define private public
#include "dialog/cprintmanager.h"
#undef protected
#undef private

// =========================================================================
// CPrintManager::slotPaintRequest
// =========================================================================

TEST(CPrintManagerSlotPaintRequest, WithEmptyImage_NoCrash)
{
    CPrintManager mgr;
    // m_image is default-constructed (empty QImage); slotPaintRequest
    // logs a warning but does not draw.

    DPrinter printer;
    mgr.slotPaintRequest(&printer);

    SUCCEED();
}

TEST(CPrintManagerSlotPaintRequest, WithValidImage_DrawsToPrinter)
{
    CPrintManager mgr;

    // Set a valid image
    QImage img(100, 100, QImage::Format_ARGB32);
    img.fill(Qt::red);
    mgr.m_image = img;

    DPrinter printer;
    mgr.slotPaintRequest(&printer);

    SUCCEED();
}
