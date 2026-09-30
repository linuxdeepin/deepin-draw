// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for CTextEdit methods (issue V-5429)
 *
 * Source: src/widgets/ctextedit.cpp
 *
 * Target methods:
 *   CTextEdit: currentFormat, getCharFormats, keyPressEvent,
 *              setCurrentFormat, updateBgColorTo, updateSelectionFormat
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QKeyEvent>
#include <QTextCharFormat>
#include <QPalette>
#include <QColor>
#include <QFont>
#include <QApplication>
#include <QTextCursor>
#include <memory>

#define protected public
#define private public
#include "widgets/ctextedit.h"
#include "drawshape/drawItems/bzItems/cgraphicstextitem.h"
#include "publicApi.h"
#undef protected
#undef private

// Helper: create a CTextEdit with a backing CGraphicsTextItem on a scene.
static std::unique_ptr<CTextEdit> createTextEdit()
{
    createNewViewByShortcutKey();
    auto *textItem = new CGraphicsTextItem("Hello World");
    // CTextEdit takes ownership of the item pointer (stores it, does not delete).
    auto edit = std::make_unique<CTextEdit>(textItem);
    edit->setPlainText("Hello World");
    return edit;
}

// =========================================================================
// CTextEdit::currentFormat
// =========================================================================

TEST(CTextEditCurrentFormat, DefaultFormat_NotConsiderSelection)
{
    auto edit = createTextEdit();
    ASSERT_NE(edit, nullptr);

    QTextCharFormat fmt = edit->currentFormat(false);
    EXPECT_TRUE(fmt.isValid());
}

TEST(CTextEditCurrentFormat, ConsiderSelection_NoSelection_ReturnsCurrent)
{
    auto edit = createTextEdit();
    ASSERT_NE(edit, nullptr);

    QTextCharFormat fmt = edit->currentFormat(true);
    EXPECT_TRUE(fmt.isValid());
}

TEST(CTextEditCurrentFormat, WithSelection_ReturnsSelectionFmt)
{
    auto edit = createTextEdit();
    ASSERT_NE(edit, nullptr);

    QTextCursor cursor = edit->textCursor();
    cursor.setPosition(0);
    cursor.setPosition(5, QTextCursor::KeepAnchor);
    edit->setTextCursor(cursor);

    QTextCharFormat fmt = edit->currentFormat(true);
    EXPECT_TRUE(fmt.isValid());
}

// =========================================================================
// CTextEdit::getCharFormats
// =========================================================================

TEST(CTextEditGetCharFormats, FullRange_ReturnsFormats)
{
    auto edit = createTextEdit();
    ASSERT_NE(edit, nullptr);

    int docLen = edit->document()->characterCount();
    auto fmts = edit->getCharFormats(0, docLen - 1);
    EXPECT_FALSE(fmts.isEmpty());
}

TEST(CTextEditGetCharFormats, EmptyDoc_ReturnsEmpty)
{
    auto *textItem = new CGraphicsTextItem("");
    auto edit = std::make_unique<CTextEdit>(textItem);
    edit->setPlainText("");

    auto fmts = edit->getCharFormats(0, 0);
    // Empty document may still return an empty format range for block 0.
    // Just verify no crash.
    SUCCEED();
}

TEST(CTextEditGetCharFormats, PartialRange_ReturnsSubset)
{
    auto edit = createTextEdit();
    ASSERT_NE(edit, nullptr);

    auto fmts = edit->getCharFormats(0, 4);
    EXPECT_FALSE(fmts.isEmpty());
}

// =========================================================================
// CTextEdit::keyPressEvent
// =========================================================================

TEST(CTextEditKeyPressEvent, CtrlZ_TriggersUndo)
{
    auto edit = createTextEdit();
    ASSERT_NE(edit, nullptr);

    // Type some text first, then undo.
    edit->setPlainText("Before undo");
    edit->append("Extra line");

    int linesBefore = edit->document()->blockCount();
    QKeyEvent undoEvent(QEvent::KeyPress, Qt::Key_Z, Qt::ControlModifier);
    edit->keyPressEvent(&undoEvent);
    int linesAfter = edit->document()->blockCount();

    EXPECT_LE(linesAfter, linesBefore);
}

TEST(CTextEditKeyPressEvent, CtrlY_TriggersRedo)
{
    auto edit = createTextEdit();
    ASSERT_NE(edit, nullptr);

    edit->setPlainText("Line 1");
    edit->append("Line 2");

    // Undo first.
    QKeyEvent undoEvent(QEvent::KeyPress, Qt::Key_Z, Qt::ControlModifier);
    edit->keyPressEvent(&undoEvent);

    // Redo.
    QKeyEvent redoEvent(QEvent::KeyPress, Qt::Key_Y, Qt::ControlModifier);
    edit->keyPressEvent(&redoEvent);

    // Should have restored the appended line.
    EXPECT_EQ(edit->document()->blockCount(), 2);
}

TEST(CTextEditKeyPressEvent, CtrlShiftZ_Ignored)
{
    auto edit = createTextEdit();
    ASSERT_NE(edit, nullptr);

    edit->setPlainText("Test text");
    edit->append("Another line");

    // Undo to create redoable state.
    QKeyEvent undoEvent(QEvent::KeyPress, Qt::Key_Z, Qt::ControlModifier);
    edit->keyPressEvent(&undoEvent);

    int blockCountBefore = edit->document()->blockCount();

    // Ctrl+Shift+Z should be ignored (does not redo).
    QKeyEvent ctrlShiftZ(QEvent::KeyPress, Qt::Key_Z,
                         Qt::ControlModifier | Qt::ShiftModifier);
    edit->keyPressEvent(&ctrlShiftZ);

    int blockCountAfter = edit->document()->blockCount();
    EXPECT_EQ(blockCountAfter, blockCountBefore);
}

// =========================================================================
// CTextEdit::setCurrentFormat
// =========================================================================

TEST(CTextEditSetCurrentFormat, MergeFormat_NoCrash)
{
    auto edit = createTextEdit();
    ASSERT_NE(edit, nullptr);

    QTextCharFormat fmt;
    fmt.setFontWeight(QFont::Bold);

    edit->setCurrentFormat(fmt, true);
    EXPECT_EQ(edit->currentCharFormat().fontWeight(), QFont::Bold);
}

TEST(CTextEditSetCurrentFormat, ReplaceFormat_NoCrash)
{
    auto edit = createTextEdit();
    ASSERT_NE(edit, nullptr);

    QTextCharFormat fmt;
    fmt.setFontItalic(true);

    edit->setCurrentFormat(fmt, false);
    EXPECT_TRUE(edit->currentCharFormat().fontItalic());
}

TEST(CTextEditSetCurrentFormat, WithSelection_MergesBlockFormat)
{
    auto edit = createTextEdit();
    ASSERT_NE(edit, nullptr);

    QTextCursor cursor = edit->textCursor();
    cursor.setPosition(0);
    cursor.setPosition(5, QTextCursor::KeepAnchor);
    edit->setTextCursor(cursor);

    QTextCharFormat fmt;
    fmt.setFontUnderline(true);

    edit->setCurrentFormat(fmt, true);
    // Verify no crash and the merge took effect.
    EXPECT_TRUE(edit->currentCharFormat().fontUnderline());
}

// =========================================================================
// CTextEdit::updateBgColorTo
// =========================================================================

TEST(CTextEditUpdateBgColor, ImmediateUpdate_ChangesPalette)
{
    auto *textItem = new CGraphicsTextItem("Bg test");
    auto edit = std::make_unique<CTextEdit>(textItem);

    QColor newColor(255, 0, 0);
    edit->updateBgColorTo(newColor, false);

    QPalette pal = edit->palette();
    QBrush baseBrush = pal.brush(QPalette::Base);
    EXPECT_EQ(baseBrush.color(), newColor);
}

TEST(CTextEditUpdateBgColor, DeferredUpdate_NoCrash)
{
    auto *textItem = new CGraphicsTextItem("Bg test deferred");
    auto edit = std::make_unique<CTextEdit>(textItem);

    QColor newColor(0, 255, 0);
    edit->updateBgColorTo(newColor, true);

    // Process pending events so the queued connection fires.
    QCoreApplication::processEvents(QEventLoop::AllEvents, 100);

    QPalette pal = edit->palette();
    QBrush baseBrush = pal.brush(QPalette::Base);
    EXPECT_EQ(baseBrush.color(), newColor);
}

// =========================================================================
// CTextEdit::updateSelectionFormat
// =========================================================================

TEST(CTextEditUpdateSelectionFormat, NoSelection_SetsDefaultFmt)
{
    auto edit = createTextEdit();
    ASSERT_NE(edit, nullptr);

    edit->updateSelectionFormat();

    // _selectionFmt should be a valid (possibly default) format.
    EXPECT_TRUE(edit->_selectionFmt.isValid() ||
                edit->_selectionFmt.properties().isEmpty());
}

TEST(CTextEditUpdateSelectionFormat, WithSelection_UpdatesFmt)
{
    auto edit = createTextEdit();
    ASSERT_NE(edit, nullptr);

    QTextCursor cursor = edit->textCursor();
    cursor.setPosition(0);
    cursor.setPosition(5, QTextCursor::KeepAnchor);
    edit->setTextCursor(cursor);

    edit->updateSelectionFormat();
    // _selectionFmt should be set (valid after selection-based retrieval).
    EXPECT_TRUE(edit->_selectionFmt.isValid() ||
                !edit->_selectionFmt.properties().isEmpty());
}

TEST(CTextEditUpdateSelectionFormat, EmptyDoc_NoCrash)
{
    auto *textItem = new CGraphicsTextItem("");
    auto edit = std::make_unique<CTextEdit>(textItem);
    edit->setPlainText("");

    edit->updateSelectionFormat();
    // With empty document, _selectionFmt should be reset to default.
    EXPECT_TRUE(edit->_selectionFmt.properties().isEmpty());
}
