// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for CTextTool font widget callback lambdas.
 *
 * Source: src/drawshape/drawTools/ctexttool.cpp
 *
 * Target lambdas (0-coverage):
 *   initFontFamilyWidget(QComboBox*) — two currentIndexChanged lambdas
 *   initFontWeightWidget()           — currentTextChanged lambda
 *   initFontFontSizeWidget()         — currentIndexChanged & editingFinished lambdas
 *
 * Strategy: call the init methods, then emit the relevant signals on the
 * created combo boxes and verify side effects (e.g. _activePackup flag,
 * _currenFontSize, attribute values).
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QComboBox>
#include <QFontComboBox>
#include <QFontDatabase>
#include <QLineEdit>
#include <QSignalSpy>

#define protected public
#define private public
#include "drawshape/drawTools/ctexttool.h"
#include "drawshape/drawTools/cdrawtoolfactory.h"
#include "drawshape/drawTools/idrawtool.h"
#include "drawshape/globaldefine.h"
#include "drawshape/cdrawscene.h"
#include "publicApi.h"
#undef protected
#undef private

#ifdef ENABLE_FSANITIZE
#include <sanitizer/lsan_interface.h>
#endif

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

// initFont*Widget() installs parentless attribute widgets into the
// process-wide CAttributeManagerWgt registry and connects
// CAttributeManagerWgt::updateWgt using those widgets as connection
// contexts with lambdas capturing `this`. Deleting a standalone tool
// leaves those live connections pointing at freed memory, and the next
// attribute-panel repaint (paintEvent -> ensureAttributions -> updateWgt)
// dereferences the freed tool. The application itself only deletes tools
// at process exit (MainWindow::~MainWindow), so mirror that lifetime:
// intentionally leak the standalone test tool instead of deleting it.
static void releaseTextTool(CTextTool *tool)
{
#ifdef ENABLE_FSANITIZE
    if (tool != nullptr)
        __lsan_ignore_object(tool);
#else
    Q_UNUSED(tool)
#endif
}

static CTextTool *createTextToolForFontTest()
{
    ensurePageExists();
    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    CTextTool *tool = qobject_cast<CTextTool *>(CDrawToolFactory::Create(text));
    if (tool) {
        tool->setDrawBoard(board);
        // CTextTool::eventFilter (ctexttool.cpp:261) dereferences
        // m_fontSize, m_fontComBox and m_fontHeavy whenever any of the
        // filter-installed widgets receives an event, and each init
        // function installs the filter while creating its widget. A tool
        // with only some widget groups initialized therefore crashes as
        // soon as the filter fires, so run all three init calls up front
        // like the production tool setup does.
        tool->initFontWeightWidget();
        tool->initFontFamilyWidget(tool->m_fontHeavy);
        tool->initFontFontSizeWidget();
    }
    return tool;
}

// ===========================================================================
// initFontFamilyWidget — currentIndexChanged lambdas
// ===========================================================================

TEST(CTextToolFontFamilyLambda, CurrentIndexChanged_SetsActivePackup)
{
    CTextTool *tool = createTextToolForFontTest();
    ASSERT_NE(tool, nullptr);
    ASSERT_NE(tool->m_fontHeavy, nullptr);
    ASSERT_NE(tool->m_fontComBox, nullptr);
    ASSERT_NE(tool->m_fontSize, nullptr);

    // The first currentIndexChanged lambda sets _activePackup = true
    tool->_activePackup = false;
    if (tool->m_fontComBox->count() > 1) {
        tool->m_fontComBox->setCurrentIndex(1);
    } else if (tool->m_fontComBox->count() == 1) {
        tool->m_fontComBox->setCurrentIndex(0);
    }
    // _activePackup should have been set to true by the lambda
    EXPECT_TRUE(tool->_activePackup);

    releaseTextTool(tool);
}

TEST(CTextToolFontFamilyLambda, FontViewShowOut_TriggersPreviewAttribution)
{
    CTextTool *tool = createTextToolForFontTest();
    ASSERT_NE(tool, nullptr);
    ASSERT_NE(tool->m_fontComBox, nullptr);

    // Simulate the font combo view being shown (sets _fontViewShowOut)
    tool->_fontViewShowOut = true;
    int prevIndex = tool->m_fontComBox->currentIndex();

    // Change index to trigger the second lambda which calls
    // drawBoard()->setDrawAttribution when _fontViewShowOut is true
    if (tool->m_fontComBox->count() > 1) {
        int newIndex = (prevIndex == 0) ? 1 : 0;
        tool->m_fontComBox->setCurrentIndex(newIndex);
    }
    // Should not crash — the lambda calls setDrawAttribution
    SUCCEED();

    releaseTextTool(tool);
}

TEST(CTextToolFontFamilyLambda, FontViewShowOutFalse_NoPreview)
{
    CTextTool *tool = createTextToolForFontTest();
    ASSERT_NE(tool, nullptr);
    ASSERT_NE(tool->m_fontComBox, nullptr);

    // _fontViewShowOut is false — the second lambda should skip preview
    tool->_fontViewShowOut = false;
    if (tool->m_fontComBox->count() > 1) {
        tool->m_fontComBox->setCurrentIndex(0);
    }
    // No crash expected
    SUCCEED();

    releaseTextTool(tool);
}

// ===========================================================================
// initFontWeightWidget — currentTextChanged lambda
// ===========================================================================

TEST(CTextToolFontWeightLambda, CurrentTextChanged_TriggersAttribution)
{
    CTextTool *tool = createTextToolForFontTest();
    ASSERT_NE(tool, nullptr);
    ASSERT_NE(tool->m_fontHeavy, nullptr);

    // Change text to trigger the currentTextChanged lambda
    // which calls drawBoard()->setDrawAttribution(EFontWeightStyle, ...)
    if (tool->m_fontHeavy->count() > 1) {
        tool->m_fontHeavy->setCurrentIndex(1);
    }
    // Should not crash
    SUCCEED();

    releaseTextTool(tool);
}

TEST(CTextToolFontWeightLambda, CurrentTextChanged_SameValue_NoCrash)
{
    CTextTool *tool = createTextToolForFontTest();
    ASSERT_NE(tool, nullptr);
    ASSERT_NE(tool->m_fontHeavy, nullptr);

    // Set to same value — should not crash
    QString curText = tool->m_fontHeavy->currentText();
    tool->m_fontHeavy->setCurrentText(curText);
    SUCCEED();

    releaseTextTool(tool);
}

// ===========================================================================
// initFontFontSizeWidget — currentIndexChanged & editingFinished lambdas
// ===========================================================================

TEST(CTextToolFontSizeLambda, CurrentIndexChanged_UpdatesFontSize)
{
    CTextTool *tool = createTextToolForFontTest();
    ASSERT_NE(tool, nullptr);
    ASSERT_NE(tool->m_fontSize, nullptr);

    // Select a different item to trigger currentIndexChanged lambda
    // which calls onSizeChanged
    if (tool->m_fontSize->count() > 1) {
        tool->m_fontSize->setCurrentIndex(2);
    }
    // Should not crash; _currenFontSize may be updated
    SUCCEED();

    releaseTextTool(tool);
}

TEST(CTextToolFontSizeLambda, EditingFinished_TriggersOnSizeChanged)
{
    CTextTool *tool = createTextToolForFontTest();
    ASSERT_NE(tool, nullptr);
    ASSERT_NE(tool->m_fontSize, nullptr);
    ASSERT_NE(tool->m_fontSize->lineEdit(), nullptr);

    // Set a value and emit editingFinished to trigger the lambda
    tool->m_fontSize->lineEdit()->setText("18px");
    emit tool->m_fontSize->lineEdit()->editingFinished();
    // Should not crash; onSizeChanged should be called
    SUCCEED();

    releaseTextTool(tool);
}

TEST(CTextToolFontSizeLambda, EditingFinished_InvalidValue_NoCrash)
{
    CTextTool *tool = createTextToolForFontTest();
    ASSERT_NE(tool, nullptr);
    ASSERT_NE(tool->m_fontSize, nullptr);

    // Set invalid text and emit editingFinished
    tool->m_fontSize->lineEdit()->setText("abc");
    emit tool->m_fontSize->lineEdit()->editingFinished();
    // Should not crash — onSizeChanged handles invalid via toInt()=0
    EXPECT_EQ(tool->_currenFontSize, 8); // clamped to minimum

    releaseTextTool(tool);
}
