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
