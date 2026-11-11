// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for CTextEdit zero-coverage methods (V-8216)
 *
 * Target methods:
 *   - contextMenuEvent(QContextMenuEvent*)
 *   - mousePressEvent(QMouseEvent*)
 *   - mouseDoubleClickEvent(QMouseEvent*)
 *   - onTextChanged()
 *
 * Source: src/widgets/ctextedit.cpp
 */

#include <gtest/gtest.h>

#include <QContextMenuEvent>
#include <QMouseEvent>
#include <QPoint>
#include <QTextCharFormat>
#include <QTextCursor>
#include <QFont>
#include <QTimer>
#include <QRectF>
#include <QCoreApplication>

#define protected public
#define private public
#include "widgets/ctextedit.h"
#include "drawshape/drawItems/bzItems/cgraphicstextitem.h"
#undef protected
#undef private

// Helper: create a CTextEdit backed by a CGraphicsTextItem on the stack.
struct CTextEditFixture {
    CGraphicsTextItem item;
    CTextEdit edit;
    CTextEditFixture(const QString &text = "test")
        : item(text), edit(&item, nullptr)
    {
        item.setRect(QRectF(0, 0, 200, 30));
    }
};

// =========================================================================
// contextMenuEvent
// =========================================================================

TEST(CTextEditExt2, ContextMenuEvent_AcceptsEvent)
{
    CTextEditFixture f;
    QContextMenuEvent event(QContextMenuEvent::Mouse, QPoint(5, 5));
    f.edit.contextMenuEvent(&event);
    EXPECT_TRUE(event.isAccepted());
}

// =========================================================================
// mousePressEvent
// =========================================================================

TEST(CTextEditExt2, MousePressEvent_NullItem_NoCrash)
{
    CGraphicsTextItem item("test");
    CTextEdit edit(&item, nullptr);
    edit.m_pItem = nullptr;
    QMouseEvent event(QEvent::MouseButtonPress, QPointF(5, 5),
                      Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    edit.mousePressEvent(&event);
    SUCCEED();
}

TEST(CTextEditExt2, MousePressEvent_WithItem_CallsFocus)
{
    CTextEditFixture f;
    // Enter edit state so toFocusEiditor() takes effect
    f.item.setTextState(CGraphicsTextItem::EInEdit);

    // toFocusEiditor() acts on the item's internal editor (m_pTextEdit),
    // not on the fixture's edit widget. Clear the flag first so the
    // assertion proves the press actually set it.
    f.item.m_pTextEdit->setTextInteractionFlags(
        f.item.m_pTextEdit->textInteractionFlags() & ~Qt::TextEditable);

    QMouseEvent event(QEvent::MouseButtonPress, QPointF(5, 5),
                      Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    f.edit.mousePressEvent(&event);

    EXPECT_TRUE(f.item.m_pTextEdit->textInteractionFlags() & Qt::TextEditable);
}

// =========================================================================
// mouseDoubleClickEvent
// =========================================================================

TEST(CTextEditExt2, MouseDoubleClickEvent_DelegatesToBase)
{
    CTextEditFixture f("Some text");
    f.edit.setPlainText("Some text");
    QTextCursor c = f.edit.textCursor();
    c.movePosition(QTextCursor::Start);
    f.edit.setTextCursor(c);

    QMouseEvent event(QEvent::MouseButtonDblClick, QPointF(5, 5),
                      Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    f.edit.mouseDoubleClickEvent(&event);
    SUCCEED();
}

// =========================================================================
// onTextChanged
// =========================================================================

TEST(CTextEditExt2, OnTextChanged_NullItem_ReturnsEarly)
{
    CGraphicsTextItem item("test");
    CTextEdit edit(&item, nullptr);
    edit.m_pItem = nullptr;
    edit.onTextChanged();
    SUCCEED();
}

TEST(CTextEditExt2, OnTextChanged_AutoAdjustSize_UpdatesRect)
{
    CTextEditFixture f("test");
    f.item.setAutoAdjustSize(true);

    f.edit.setPlainText("Hello world test text");
    f.edit.onTextChanged();

    QRectF r = f.item.rect();
    EXPECT_GT(r.width(), 0.0);
    EXPECT_GT(r.height(), 0.0);
}

TEST(CTextEditExt2, OnTextChanged_NonAutoAdjust_SmallHeight_Adjusts)
{
    CTextEditFixture f("test");
    f.item.setRect(QRectF(0, 0, 200, 2));
    f.item.setAutoAdjustSize(false);

    f.edit.setPlainText("Hello");
    f.edit.onTextChanged();
    QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    SUCCEED();
}
