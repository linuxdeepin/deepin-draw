// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for CTextEdit slot/method coverage (issue V-5860)
 *
 * Source: src/widgets/ctextedit.cpp
 *
 * Target methods:
 *   firstPosFormat, setDefaultFormat, applyDefaultToFirstFormat,
 *   insertFromMimeData, onTextChanged, onCursorPositionChanged,
 *   onSelectionChanged, onCurrentCharFormatChanged, updatePropertyWidget
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QKeyEvent>
#include <QTextCharFormat>
#include <QFont>
#include <QApplication>
#include <QTextCursor>
#include <QMimeData>
#include <QTimer>
#include <memory>

#define protected public
#define private public
#include "widgets/ctextedit.h"
#include "drawshape/drawItems/bzItems/cgraphicstextitem.h"
#include "publicApi.h"
#undef protected
#undef private

// CTextEdit does not own its CGraphicsTextItem (its destructor only clears
// the pointer), so tests must delete both. The item also creates its own
// editor (m_pTextEdit) in its constructor, which is parentless and would
// otherwise only be freed via deleteLater(), so delete it explicitly here.
static void deleteEditAndItem(CTextEdit *e)
{
    CGraphicsTextItem *item = e->m_pItem;
    delete e;
    delete item->m_pTextEdit;
    item->m_pTextEdit = nullptr;
    delete item;
}

// Helper: create a CTextEdit with a backing CGraphicsTextItem on a fresh view.
static std::unique_ptr<CTextEdit, void (*)(CTextEdit *)>
createTextEdit(const QString &text = "Hello World")
{
    createNewViewByShortcutKey();
    auto *textItem = new CGraphicsTextItem(text);
    std::unique_ptr<CTextEdit, void (*)(CTextEdit *)> edit(
        new CTextEdit(textItem), deleteEditAndItem);
    edit->setPlainText(text);
    return edit;
}

// =========================================================================
// firstPosFormat
// =========================================================================

TEST(CTextEditSlotsTest, FirstPosFormatWithContent)
{
    auto edit = createTextEdit("Hello");
    QTextCharFormat fmt = edit->firstPosFormat();
    EXPECT_TRUE(fmt.isValid());
}

TEST(CTextEditSlotsTest, FirstPosFormatEmptyDoc)
{
    auto edit = createTextEdit("");
    // empty document → returns currentCharFormat()
    QTextCharFormat fmt = edit->firstPosFormat();
    EXPECT_TRUE(fmt.isValid());
}

// =========================================================================
// setDefaultFormat
// =========================================================================

TEST(CTextEditSlotsTest, SetDefaultFormatStoresAndApplies)
{
    auto edit = createTextEdit("Test");

    QTextCharFormat fmt;
    fmt.setFontWeight(QFont::Bold);
    fmt.setFontPointSize(16.0);

    edit->setDefaultFormat(fmt);

    // _defaultFormat should be set
    EXPECT_EQ(edit->_defaultFormat.fontWeight(), QFont::Bold);
    // cursor should be at position 0
    EXPECT_EQ(edit->textCursor().position(), 0);
}

// =========================================================================
// applyDefaultToFirstFormat
// =========================================================================

TEST(CTextEditSlotsTest, ApplyDefaultToFirstFormat)
{
    auto edit = createTextEdit("ABC");

    QTextCharFormat boldFmt;
    boldFmt.setFontWeight(QFont::Bold);

    // Set the first block format to bold via cursor
    QTextCursor cur = edit->textCursor();
    cur.movePosition(QTextCursor::Start);
    cur.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, 1);
    cur.mergeCharFormat(boldFmt);

    edit->applyDefaultToFirstFormat();

    // _defaultFormat should now have Bold (from firstPosFormat)
    EXPECT_EQ(edit->_defaultFormat.fontWeight(), QFont::Bold);
}

// =========================================================================
// insertFromMimeData
// =========================================================================

TEST(CTextEditSlotsTest, InsertFromMimeDataText)
{
    auto edit = createTextEdit("Hello");
    edit->setPlainText("");

    QMimeData *mime = new QMimeData;
    mime->setText("pasted text");
    edit->insertFromMimeData(mime);

    EXPECT_EQ(edit->toPlainText(), "pasted text");
    delete mime;
}

TEST(CTextEditSlotsTest, InsertFromMimeDataNoText)
{
    auto edit = createTextEdit("");
    edit->setPlainText("existing");

    // MimeData with no text → falls through to QTextEdit::insertFromMimeData
    QMimeData *mime = new QMimeData;
    mime->setHtml("<b>html</b>");
    edit->insertFromMimeData(mime);

    // The text should not be "pasted text" (plain text path not taken)
    EXPECT_NE(edit->toPlainText(), "");
    delete mime;
}

TEST(CTextEditSlotsTest, InsertFromMimeDataNull)
{
    auto edit = createTextEdit("Hello");
    // Should not crash with null
    edit->insertFromMimeData(nullptr);
    SUCCEED();
}

// =========================================================================
// onTextChanged
// =========================================================================

TEST(CTextEditSlotsTest, OnTextChangedWithAutoAdjustSize)
{
    auto edit = createTextEdit("Hi");
    edit->m_pItem->setAutoAdjustSize(true);

    edit->setPlainText("New longer text");

    // onTextChanged is connected via signal, so it was already called.
    // Verify the item rect was adjusted.
    EXPECT_GT(edit->m_pItem->rect().width(), 0);
}

TEST(CTextEditSlotsTest, OnTextChangedNullItemNoCrash)
{
    auto edit = createTextEdit("Hi");
    CGraphicsTextItem *item = edit->m_pItem;
    {
        struct RestoreItem {
            CTextEdit *edit;
            CGraphicsTextItem *saved;
            ~RestoreItem() { edit->m_pItem = saved; }
        } guard{edit.get(), item};
        edit->m_pItem = nullptr;

        // Directly call the slot — should return early
        edit->onTextChanged();
        SUCCEED();
    }
}

TEST(CTextEditSlotsTest, OnTextChangedWithoutAutoAdjustSize)
{
    auto edit = createTextEdit("Hi");
    edit->m_pItem->setAutoAdjustSize(false);

    // Set a very small rect so font height > widget height triggers resize
    edit->m_pItem->setRect(QRectF(0, 0, 10, 1));

    edit->setPlainText("Text that might be taller than rect");

    // Should not crash; the slot checks font height vs widget height
    SUCCEED();
}

// =========================================================================
// onCursorPositionChanged
// =========================================================================

TEST(CTextEditSlotsTest, OnCursorPositionChangedNoSelection)
{
    auto edit = createTextEdit("Hello World");
    edit->_updateTimer = nullptr;

    QTextCursor cur = edit->textCursor();
    cur.setPosition(3);
    edit->setTextCursor(cur);

    // The slot is connected to cursorPositionChanged signal.
    // With no selection, it should call updatePropertyWidget which
    // creates the timer.
    EXPECT_NE(edit->_updateTimer, nullptr);
}

TEST(CTextEditSlotsTest, OnCursorPositionChangedWithSelection)
{
    auto edit = createTextEdit("Hello World");
    edit->_updateTimer = nullptr;

    QTextCursor cur = edit->textCursor();
    cur.setPosition(0);
    cur.setPosition(5, QTextCursor::KeepAnchor);
    edit->setTextCursor(cur);

    // onCursorPositionChanged has a guard: if textCursor().hasSelection()
    // it skips updatePropertyWidget(). However setTextCursor with a
    // selection also fires selectionChanged -> onSelectionChanged which
    // calls updatePropertyWidget() unconditionally, so the timer IS
    // created. We verify the timer was created (by onSelectionChanged)
    // rather than by onCursorPositionChanged.
    EXPECT_NE(edit->_updateTimer, nullptr);
}

// =========================================================================
// onSelectionChanged
// =========================================================================

TEST(CTextEditSlotsTest, OnSelectionChangedTriggersUpdate)
{
    auto edit = createTextEdit("Hello World");
    edit->_updateTimer = nullptr;

    // Make a selection to trigger onSelectionChanged → updatePropertyWidget
    QTextCursor cur = edit->textCursor();
    cur.setPosition(0);
    cur.setPosition(3, QTextCursor::KeepAnchor);
    edit->setTextCursor(cur);

    // updatePropertyWidget should have been called → timer created
    EXPECT_NE(edit->_updateTimer, nullptr);
}

// =========================================================================
// onCurrentCharFormatChanged
// =========================================================================

TEST(CTextEditSlotsTest, OnCurrentCharFormatChanged)
{
    auto edit = createTextEdit("Hello");
    edit->_updateTimer = nullptr;

    // Change the char format to trigger currentCharFormatChanged signal
    QTextCharFormat fmt;
    fmt.setFontWeight(QFont::Bold);
    QTextCursor cur = edit->textCursor();
    cur.select(QTextCursor::Document);
    cur.mergeCharFormat(fmt);
    edit->mergeCurrentCharFormat(fmt);

    // onCurrentCharFormatChanged calls updatePropertyWidget → timer created
    EXPECT_NE(edit->_updateTimer, nullptr);
}

// =========================================================================
// updatePropertyWidget
// =========================================================================

TEST(CTextEditSlotsTest, UpdatePropertyWidgetCreatesTimer)
{
    auto edit = createTextEdit("Hello");
    // createTextEdit triggers signals that may already create the timer;
    // reset it to verify updatePropertyWidget creates a fresh one.
    edit->_updateTimer = nullptr;

    edit->updatePropertyWidget();

    ASSERT_NE(edit->_updateTimer, nullptr);
    EXPECT_TRUE(edit->_updateTimer->isSingleShot());
}

TEST(CTextEditSlotsTest, UpdatePropertyWidgetStartsTimer)
{
    auto edit = createTextEdit("Hello");
    edit->updatePropertyWidget();

    // Timer should be active (started with 70ms)
    EXPECT_TRUE(edit->_updateTimer->isActive());
}

TEST(CTextEditSlotsTest, UpdatePropertyWidgetReusesTimer)
{
    auto edit = createTextEdit("Hello");
    edit->updatePropertyWidget();
    QTimer *firstTimer = edit->_updateTimer;

    edit->updatePropertyWidget();

    // Should reuse the same timer, not create a new one
    EXPECT_EQ(edit->_updateTimer, firstTimer);
}
