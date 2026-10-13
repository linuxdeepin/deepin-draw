// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for CTextTool private methods (ctexttool.cpp).
 *
 * Target functions (from issue V-5568):
 *   - CTextTool::initFontFamilyWidget(QComboBox *)
 *   - CTextTool::reInitFontWeightComboxItems(const QString &, QComboBox *)
 *   - CTextTool::resetItemsFontFamily()
 *
 * Target functions (from issue V-5645):
 *   - CTextTool::initFontWeightWidget()
 *   - CTextTool::minMoveUpdateDistance()
 *   - CTextTool::isTextEnableUndoThisTime(bool)
 *
 * All three are private; access via #define private public.
 * initFontFamilyWidget requires a running application (drawApp).
 * reInitFontWeightComboxItems and resetItemsFontFamily can be tested
 * with a CTextTool instance obtained from the tool factory.
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QComboBox>
#include <QFontDatabase>
#include <QApplication>

// Include Qt headers BEFORE #define private to avoid breaking Qt internals.
#include <QKeyEvent>
#include <QEvent>

#define protected public
#define private public
#include "drawshape/drawTools/ctexttool.h"
#include "drawshape/globaldefine.h"
#include "drawshape/drawTools/cdrawtoolfactory.h"
#include "drawshape/drawTools/idrawtool.h"
#include "publicApi.h"
#undef protected
#undef private

// =========================================================================
// CTextTool::reInitFontWeightComboxItems
// =========================================================================

TEST(CTextToolReInitFontWeightTest, PopulatesComboBoxForKnownFamily)
{
    createNewViewByShortcutKey();
    drawApp->setCurrentTool(text);

    CTextTool *textTool = dynamic_cast<CTextTool *>(
        CDrawToolFactory::tool(text));
    ASSERT_NE(textTool, nullptr);

    QComboBox fontHeavy;
    fontHeavy.addItems(textTool->supWeightStyleList);

    // Pick a family that has a known "Regular" style so the expected
    // combo box contents are predictable.
    QFontDatabase db;
    QString family;
    for (const QString &f : db.families()) {
        if (db.styles(f).contains("Regular")) {
            family = f;
            break;
        }
    }
    ASSERT_FALSE(family.isEmpty()) << "no installed family provides a Regular style";

    // Expected items: the supported weight styles this family actually
    // has, in supWeightStyleList order.
    QStringList expected;
    const QStringList familyStyles = db.styles(family);
    for (const QString &style : textTool->supWeightStyleList) {
        if (familyStyles.contains(style))
            expected << style;
    }
    ASSERT_TRUE(expected.contains("Regular"));

    textTool->reInitFontWeightComboxItems(family, &fontHeavy);

    // The combo box must contain exactly the expected styles.
    ASSERT_EQ(fontHeavy.count(), expected.count());
    for (int i = 0; i < expected.count(); ++i) {
        EXPECT_EQ(fontHeavy.itemText(i), expected.at(i));
    }
}

TEST(CTextToolReInitFontWeightTest, EmptyFamilyClearsComboBox)
{
    createNewViewByShortcutKey();
    drawApp->setCurrentTool(text);

    CTextTool *textTool = dynamic_cast<CTextTool *>(
        CDrawToolFactory::tool(text));
    ASSERT_NE(textTool, nullptr);

    QComboBox fontHeavy;
    fontHeavy.addItems(textTool->supWeightStyleList);
    ASSERT_GT(fontHeavy.count(), 0);

    textTool->reInitFontWeightComboxItems("NonExistentFont12345", &fontHeavy);

    // No styles match, so the combo box should be empty
    EXPECT_EQ(fontHeavy.count(), 0);
}

TEST(CTextToolReInitFontWeightTest, RetainsCurrentWeightIfAvailable)
{
    createNewViewByShortcutKey();
    drawApp->setCurrentTool(text);

    CTextTool *textTool = dynamic_cast<CTextTool *>(
        CDrawToolFactory::tool(text));
    ASSERT_NE(textTool, nullptr);

    QComboBox fontHeavy;
    fontHeavy.addItems(textTool->supWeightStyleList);
    fontHeavy.setCurrentText("Regular");

    // Pick a family that has a "Regular" style so retention is verifiable.
    QFontDatabase db;
    QString family;
    for (const QString &f : db.families()) {
        if (db.styles(f).contains("Regular")) {
            family = f;
            break;
        }
    }
    ASSERT_FALSE(family.isEmpty()) << "no installed family provides a Regular style";

    textTool->reInitFontWeightComboxItems(family, &fontHeavy);

    // Regular must remain selected after reinitialization.
    EXPECT_GE(fontHeavy.currentIndex(), 0);
    EXPECT_EQ(fontHeavy.currentText(), QString("Regular"));
}

// =========================================================================
// CTextTool::resetItemsFontFamily
// =========================================================================

TEST(CTextToolResetFontFamilyTest, ResetClearsCache)
{
    createNewViewByShortcutKey();
    drawApp->setCurrentTool(text);

    CTextTool *textTool = dynamic_cast<CTextTool *>(
        CDrawToolFactory::tool(text));
    ASSERT_NE(textTool, nullptr);

    // resetItemsFontFamily clears _cachedFontFamily
    textTool->resetItemsFontFamily();
    EXPECT_TRUE(textTool->_cachedFontFamily.isEmpty());
}

TEST(CTextToolResetFontFamilyTest, ResetAfterCachePopulated)
{
    createNewViewByShortcutKey();
    drawApp->setCurrentTool(text);

    CTextTool *textTool = dynamic_cast<CTextTool *>(
        CDrawToolFactory::tool(text));
    ASSERT_NE(textTool, nullptr);

    // Call cachedItemsFontFamily which populates the cache, then reset
    textTool->cachedItemsFontFamily();
    textTool->resetItemsFontFamily();
    EXPECT_TRUE(textTool->_cachedFontFamily.isEmpty());
}

// =========================================================================
// CTextTool::initFontFamilyWidget (requires drawApp)
// =========================================================================

TEST(CTextToolInitFontFamilyTest, InitDoesNotCrash)
{
    createNewViewByShortcutKey();
    drawApp->setCurrentTool(text);

    CTextTool *textTool = dynamic_cast<CTextTool *>(
        CDrawToolFactory::tool(text));
    ASSERT_NE(textTool, nullptr);

    // Heap-allocate: initFontFamilyWidget captures this pointer in connect
    // lambdas owned by the persistent CTextTool / attribution widgets, so the
    // combo box must outlive this test (a stack object would dangle and crash
    // later tests when the lambdas run).
    QComboBox *fontHeavy = new QComboBox;
    textTool->initFontFamilyWidget(fontHeavy);

    // After init, m_fontComBox should be set
    EXPECT_NE(textTool->m_fontComBox, nullptr);

    // The font combo box should have font families
    EXPECT_GT(textTool->m_fontComBox->count(), 0);
}

// =========================================================================
// Fixture for V-5645 tests: creates a view, activates the text tool and
// exposes the singleton CTextTool instance as `textTool`.
// =========================================================================

class CTextToolSetupFixture : public ::testing::Test
{
protected:
    void SetUp() override
    {
        createNewViewByShortcutKey();
        drawApp->setCurrentTool(text);
        textTool = dynamic_cast<CTextTool *>(CDrawToolFactory::tool(text));
        ASSERT_NE(textTool, nullptr);
    }

    CTextTool *textTool = nullptr;
};

// =========================================================================
// CTextTool::minMoveUpdateDistance (V-5645)
// =========================================================================

TEST_F(CTextToolSetupFixture, MinMoveUpdateDistance_ReturnsZero)
{
    EXPECT_EQ(textTool->minMoveUpdateDistance(), 0);
}

TEST_F(CTextToolSetupFixture, MinMoveUpdateDistance_ReturnsZeroAfterToolChange)
{
    drawApp->setCurrentTool(rectangle);
    drawApp->setCurrentTool(text);

    EXPECT_EQ(textTool->minMoveUpdateDistance(), 0);
}

TEST_F(CTextToolSetupFixture, MinMoveUpdateDistance_ConsistentReturnAcrossCalls)
{
    int first = textTool->minMoveUpdateDistance();
    int second = textTool->minMoveUpdateDistance();
    EXPECT_EQ(first, second);
}

// =========================================================================
// CTextTool::isTextEnableUndoThisTime (V-5645)
// =========================================================================

TEST_F(CTextToolSetupFixture, IsTextEnableUndoThisTime_NoActiveTextItem_ReturnsTrue)
{
    // With no active text item being edited, undo should be enabled.
    EXPECT_TRUE(textTool->isTextEnableUndoThisTime(false));
}

TEST_F(CTextToolSetupFixture, IsTextEnableUndoThisTime_ConsiderRecorderEmpty_NoRecord_ReturnsTrue)
{
    // considerRecorderEmpty=true: a fresh view has an empty undo
    // stack, so undo is reported as enabled.
    EXPECT_TRUE(textTool->isTextEnableUndoThisTime(true));
}

TEST_F(CTextToolSetupFixture, IsTextEnableUndoThisTime_NoCurrentPage_ReturnsFalse)
{
    // Detach the draw board so currentPage() returns nullptr —
    // isTextEnableUndoThisTime() must return false in that case.
    DrawBoard *board = textTool->drawBoard();
    ASSERT_NE(board, nullptr);
    textTool->setDrawBoard(nullptr);

    EXPECT_FALSE(textTool->isTextEnableUndoThisTime(false));
    EXPECT_FALSE(textTool->isTextEnableUndoThisTime(true));

    // Restore the board so later tests are unaffected.
    textTool->setDrawBoard(board);
}

// =========================================================================
// CTextTool::initFontWeightWidget (V-5645)
// =========================================================================

TEST_F(CTextToolSetupFixture, InitFontWeightWidget_DoesNotCrash)
{
    // initFontWeightWidget is called during tool setup; calling it
    // again should not crash.
    textTool->initFontWeightWidget();

    // m_fontHeavy should be set after initialization.
    QComboBox *combo = textTool->m_fontHeavy;
    EXPECT_NE(combo, nullptr);

    // The combo box should contain the standard weight style items.
    EXPECT_GT(combo->count(), 0);
}

TEST_F(CTextToolSetupFixture, InitFontWeightWidget_ComboContainsStandardStyles)
{
    textTool->initFontWeightWidget();

    QComboBox *combo = textTool->m_fontHeavy;
    ASSERT_NE(combo, nullptr);

    // Check that at least "Regular" and "Bold" are present.
    bool hasRegular = false;
    bool hasBold = false;
    for (int i = 0; i < combo->count(); ++i) {
        if (combo->itemText(i) == "Regular")
            hasRegular = true;
        if (combo->itemText(i) == "Bold")
            hasBold = true;
    }
    EXPECT_TRUE(hasRegular);
    EXPECT_TRUE(hasBold);
}

TEST_F(CTextToolSetupFixture, InitFontWeightWidget_ReInitDoesNotCrash)
{
    // Call initFontWeightWidget twice — should not crash or leak.
    textTool->initFontWeightWidget();
    textTool->initFontWeightWidget();

    EXPECT_NE(textTool->m_fontHeavy, nullptr);
}
