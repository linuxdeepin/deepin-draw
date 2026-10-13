// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for CTextEdit methods (issue V-5645)
 *
 * Source: src/widgets/ctextedit.cpp
 *
 * Target methods:
 *   CTextEdit: currentColor()/setCurrentColor(...),
 *               currentFontSize()/setCurrentFontSize(...),
 *               currentFontFamily()/setCurrentFontFamily(...),
 *               currentFontStyle()/setCurrentFontStyle(...)
 *
 * NOTE: toWeight()/toStyle() are already covered in
 *       tests/testItems/test_ctextedit.cpp — not duplicated here.
 *
 * Pattern: follows test_ctextedit_funcs.cpp — requires app instance,
 *          uses createNewViewByShortcutKey() + CGraphicsTextItem.
 *
 * Implementation note:
 *   currentFormat() has default considerSelection=true.  When the
 *   text item is in EReadOnly state, currentFormat(true) calls
 *   selectAll() then updateSelectionFormat(), returning _selectionFmt
 *   computed from the selected text.  Therefore setCurrentXxx() must
 *   be called *after* selectAll() so that mergeBlockCharFormat applies
 *   the format to the selected text and the getter roundtrip works.
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QColor>
#include <QFont>
#include <QTextCharFormat>
#include <QTextCursor>
#include <QFontDatabase>
#include <memory>

#define protected public
#define private public
#include "widgets/ctextedit.h"
#include "drawshape/drawItems/bzItems/cgraphicstextitem.h"
#include "publicApi.h"
#undef protected
#undef private

// CTextEdit does not own its CGraphicsTextItem; both must be cleaned up.
// The item creates its own editor (m_pTextEdit) in its constructor.
static void deleteEditAndItem(CTextEdit *e)
{
    CGraphicsTextItem *item = e->m_pItem;
    delete e;
    if (item->m_pTextEdit) {
        delete item->m_pTextEdit;
        item->m_pTextEdit = nullptr;
    }
    delete item;
}

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
// CTextEdit::currentColor / setCurrentColor
// =========================================================================

TEST(CTextEditColor, DefaultColor_IsInvalid)
{
    auto edit = createTextEdit();
    ASSERT_NE(edit, nullptr);

    QColor color = edit->currentColor();
    EXPECT_FALSE(color.isValid());
}

TEST(CTextEditColor, SetCurrentColor_ThenGet)
{
    auto edit = createTextEdit();
    ASSERT_NE(edit, nullptr);

    edit->selectAll();
    QColor red(255, 0, 0);
    edit->setCurrentColor(red);

    QColor got = edit->currentColor();
    EXPECT_EQ(got, red);
}

TEST(CTextEditColor, SetCurrentColor_Black)
{
    auto edit = createTextEdit();
    ASSERT_NE(edit, nullptr);

    edit->selectAll();
    edit->setCurrentColor(Qt::black);
    EXPECT_EQ(edit->currentColor(), Qt::black);
}

// =========================================================================
// CTextEdit::currentFontSize / setCurrentFontSize
// =========================================================================

TEST(CTextEditFontSize, DefaultFontSize_IsZero)
{
    auto edit = createTextEdit();
    ASSERT_NE(edit, nullptr);

    // No font size explicitly set — default is 0 (invalid point size).
    EXPECT_EQ(edit->currentFontSize(), 0);
}

TEST(CTextEditFontSize, SetCurrentFontSize_ThenGet)
{
    auto edit = createTextEdit();
    ASSERT_NE(edit, nullptr);

    edit->selectAll();
    edit->setCurrentFontSize(24);
    EXPECT_EQ(edit->currentFontSize(), 24);
}

TEST(CTextEditFontSize, SetCurrentFontSize_LargeValue)
{
    auto edit = createTextEdit();
    ASSERT_NE(edit, nullptr);

    edit->selectAll();
    edit->setCurrentFontSize(72);
    EXPECT_EQ(edit->currentFontSize(), 72);
}

TEST(CTextEditFontSize, SetCurrentFontSize_Overwrite)
{
    auto edit = createTextEdit();
    ASSERT_NE(edit, nullptr);

    edit->selectAll();
    edit->setCurrentFontSize(12);
    edit->selectAll();
    edit->setCurrentFontSize(48);
    EXPECT_EQ(edit->currentFontSize(), 48);
}

// =========================================================================
// CTextEdit::currentFontFamily / setCurrentFontFamily
// =========================================================================

TEST(CTextEditFontFamily, SetCurrentFontFamily_ThenGet)
{
    auto edit = createTextEdit();
    ASSERT_NE(edit, nullptr);

    edit->selectAll();
    QString family = QFontDatabase::families().value(0, "Sans Serif");
    edit->setCurrentFontFamily(family);

    QString got = edit->currentFontFamily();
    EXPECT_FALSE(got.isEmpty());
}

TEST(CTextEditFontFamily, SetCurrentFontFamily_Generic)
{
    auto edit = createTextEdit();
    ASSERT_NE(edit, nullptr);

    edit->selectAll();
    edit->setCurrentFontFamily("Sans Serif");
    QString got = edit->currentFontFamily();
    // The font family may be normalized; just verify it was set.
    EXPECT_FALSE(got.isEmpty());
}

TEST(CTextEditFontFamily, DefaultFamily_NoCrash)
{
    auto edit = createTextEdit();
    ASSERT_NE(edit, nullptr);

    QString family = edit->currentFontFamily();
    // Default font family may or may not be set depending on platform.
    // Just verify no crash.
    SUCCEED();
}

// =========================================================================
// CTextEdit::currentFontStyle / setCurrentFontStyle
// =========================================================================

TEST(CTextEditFontStyle, SetCurrentFontStyle_Bold)
{
    auto edit = createTextEdit();
    ASSERT_NE(edit, nullptr);

    edit->selectAll();
    edit->setCurrentFontStyle("Bold");
    QString got = edit->currentFontStyle();
    EXPECT_EQ(got.toStdString(), "Bold");
}

TEST(CTextEditFontStyle, SetCurrentFontStyle_Thin)
{
    auto edit = createTextEdit();
    ASSERT_NE(edit, nullptr);

    edit->selectAll();
    edit->setCurrentFontStyle("Thin");
    QString got = edit->currentFontStyle();
    EXPECT_EQ(got.toStdString(), "Thin");
}

TEST(CTextEditFontStyle, SetCurrentFontStyle_Regular)
{
    auto edit = createTextEdit();
    ASSERT_NE(edit, nullptr);

    edit->selectAll();
    edit->setCurrentFontStyle("Regular");
    QString got = edit->currentFontStyle();
    EXPECT_EQ(got.toStdString(), "Regular");
}

TEST(CTextEditFontStyle, SetCurrentFontStyle_Unknown_ReturnsThin)
{
    auto edit = createTextEdit();
    ASSERT_NE(edit, nullptr);

    edit->selectAll();
    edit->setCurrentFontStyle("NonExistent");
    // toWeight("NonExistent") returns 0, toStyle(0) returns "Thin".
    // So the round-trip maps unknown to "Thin".
    QString got = edit->currentFontStyle();
    EXPECT_EQ(got.toStdString(), "Thin");
}

TEST(CTextEditFontStyle, RoundTrip_AllStandardStyles)
{
    auto edit = createTextEdit();
    ASSERT_NE(edit, nullptr);

    const char *styles[] = {"Thin",    "ExtraLight", "Light",
                            "Regular", "Medium",     "DemiBold",
                            "Bold",    "ExtraBold",  "Black"};

    for (const char *s : styles) {
        edit->selectAll();
        edit->setCurrentFontStyle(QString::fromLatin1(s));
        QString got = edit->currentFontStyle();
        EXPECT_EQ(got.toStdString(), s) << "style: " << s;
    }
}
