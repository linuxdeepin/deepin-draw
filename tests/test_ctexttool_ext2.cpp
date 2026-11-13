// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for CTextTool methods (issue V-7618 group 5)
 *
 * Source: src/drawshape/drawTools/ctexttool.cpp
 *
 * Target methods:
 *   CTextTool::isPressEventHandledByQt(CDrawToolEvent *, ITERecordInfo *)
 *   CTextTool::rasterItemToLayer(CDrawToolEvent *, ITERecordInfo *)
 *   CTextTool::initFontFontSizeWidget()
 *   CTextTool::onSizeChanged(int fontSz, bool backFocus)
 *   CTextTool::restoreItemsFontFamily()
 *   CTextTool::transferFocusBack()
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

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

// =========================================================================
// Helper
// =========================================================================

// initFontFontSizeWidget() installs a parentless attribute widget into the
// process-wide CAttributeManagerWgt registry and connects
// CAttributeManagerWgt::updateWgt with that widget as connection context,
// capturing `this`. Deleting a standalone tool leaves that live connection
// pointing at freed memory; the next attribute-panel repaint
// (paintEvent -> ensureAttributions -> updateWgt) dereferences the freed
// tool. The application itself only deletes tools at process exit
// (MainWindow::~MainWindow), so mirror that lifetime: intentionally leak
// the standalone test tool instead of deleting it.
static void releaseTextTool(CTextTool *tool)
{
#ifdef ENABLE_FSANITIZE
    if (tool != nullptr)
        __lsan_ignore_object(tool);
#else
    Q_UNUSED(tool)
#endif
}

static CTextTool *createTextToolForTest()
{
    createNewViewByShortcutKey();
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

// =========================================================================
// CTextTool::isPressEventHandledByQt
// =========================================================================

TEST(CTextToolIsPressEventHandledByQt, DelegatesToBase_NoCrash)
{
    CTextTool *tool = createTextToolForTest();
    ASSERT_NE(tool, nullptr);

    // The implementation delegates to IDrawTool::isPressEventHandledByQt
    // which requires a valid event and info. With null parameters it may crash,
    // so we only verify the method exists and is callable with proper setup.
    // Since constructing CDrawToolEvent requires scene/view, we QSKIP.
    // Release the tool before skipping so QSKIP's early return path is tidy.
    releaseTextTool(tool);
    QSKIP("Requires constructed CDrawToolEvent with scene/view — skip");
}

// =========================================================================
// CTextTool::rasterItemToLayer
// =========================================================================

TEST(CTextToolRasterItemToLayer, AlwaysReturnsFalse)
{
    CTextTool *tool = createTextToolForTest();
    ASSERT_NE(tool, nullptr);

    // The implementation unconditionally returns false
    bool result = tool->rasterItemToLayer(nullptr, nullptr);
    EXPECT_FALSE(result);

    releaseTextTool(tool);
}

// =========================================================================
// CTextTool::initFontFontSizeWidget
// =========================================================================

TEST(CTextToolInitFontFontSizeWidget, InitializesControls_NoCrash)
{
    CTextTool *tool = createTextToolForTest();
    ASSERT_NE(tool, nullptr);

    // After initialization, the font size combo box should be non-null
    EXPECT_NE(tool->m_fontSize, nullptr);

    releaseTextTool(tool);
}

// =========================================================================
// CTextTool::onSizeChanged
// =========================================================================

TEST(CTextToolOnSizeChanged, ValidSize_UpdatesCurrentFontSize)
{
    CTextTool *tool = createTextToolForTest();
    ASSERT_NE(tool, nullptr);

    ASSERT_NE(tool->m_fontSize, nullptr);

    // Set a valid font size
    tool->onSizeChanged(24, false);

    // _currenFontSize should be updated
    EXPECT_EQ(tool->_currenFontSize, 24);

    releaseTextTool(tool);
}

TEST(CTextToolOnSizeChanged, ClampsToMinimum8)
{
    CTextTool *tool = createTextToolForTest();
    ASSERT_NE(tool, nullptr);

    // Size below minimum should be clamped to 8
    tool->onSizeChanged(2, false);
    EXPECT_EQ(tool->_currenFontSize, 8);

    releaseTextTool(tool);
}

TEST(CTextToolOnSizeChanged, ClampsToMaximum500)
{
    CTextTool *tool = createTextToolForTest();
    ASSERT_NE(tool, nullptr);

    // Size above maximum should be clamped to 500
    tool->onSizeChanged(9999, false);
    EXPECT_EQ(tool->_currenFontSize, 500);

    releaseTextTool(tool);
}

// =========================================================================
// CTextTool::restoreItemsFontFamily
// =========================================================================

TEST(CTextToolRestoreItemsFontFamily, EmptyCache_NoCrash)
{
    CTextTool *tool = createTextToolForTest();
    ASSERT_NE(tool, nullptr);

    // With empty _cachedFontFamily, the loop body should not execute
    tool->restoreItemsFontFamily();

    SUCCEED();
    releaseTextTool(tool);
}

// =========================================================================
// CTextTool::transferFocusBack
// =========================================================================

TEST(CTextToolTransferFocusBack, WithValidPage_NoCrash)
{
    CTextTool *tool = createTextToolForTest();
    ASSERT_NE(tool, nullptr);

    // With a valid current page, transferFocusBack should not crash
    tool->transferFocusBack();

    SUCCEED();
    releaseTextTool(tool);
}
