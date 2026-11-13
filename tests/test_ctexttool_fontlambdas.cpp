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
 * created combo boxes and verify side effects (the _activePackup flag,
 * _currenFontSize, and the page's default attribute values).
 *
 * The CTextTool source file is not modified: the tests only drive its
 * public/private members through the access-adjusting macros.
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

#include "test_helpers.h"

#ifdef ENABLE_FSANITIZE
#include <sanitizer/lsan_interface.h>
#endif

namespace {

// onSizeChanged() clamps the requested size to this range
// (ctexttool.cpp:111).
constexpr int kMinFontSize = 8;
constexpr int kMaxFontSize = 500;
// Index of the "12px" entry inside _defaultFontSizeSet.
constexpr int kSizeIndexTwelve = 2;
constexpr int kFontSizeTwelve = 12;
constexpr int kFontSizeEighteen = 18;

// initFont*Widget() installs parentless attribute widgets into the
// process-wide CAttributeManagerWgt registry and connects
// CAttributeManagerWgt::updateWgt using those widgets as connection
// contexts with lambdas capturing `this`. Deleting a standalone tool
// leaves those live connections pointing at freed memory, and the next
// attribute-panel repaint (paintEvent -> ensureAttributions -> updateWgt)
// dereferences the freed tool. The application itself only deletes tools
// at process exit (MainWindow::~MainWindow), so mirror that lifetime:
// intentionally leak the standalone test tool instead of deleting it.
void releaseTextTool(CTextTool *tool)
{
#ifdef ENABLE_FSANITIZE
    if (tool != nullptr)
        __lsan_ignore_object(tool);
#else
    Q_UNUSED(tool)
#endif
}

CTextTool *createTextToolForFontTest()
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

// The lambdas under test write the page's default attributes; preserve the
// harness page's values and put them back so later suites see the same
// defaults they would have without these tests.
class CTextToolFontLambdaTest : public PageStateGuard
{
protected:
    void SetUp() override
    {
        PageStateGuard::SetUp();
        m_page = drawApp->drawBoard()->currentPage();
        ASSERT_NE(m_page, nullptr);
        m_fontFamilyBefore = m_page->defaultAttriVar(DrawAttribution::EFontFamily);
        m_fontWeightBefore = m_page->defaultAttriVar(DrawAttribution::EFontWeightStyle);
        m_fontSizeBefore = m_page->defaultAttriVar(DrawAttribution::EFontSize);
    }

    void TearDown() override
    {
        if (m_page != nullptr) {
            m_page->setDefaultAttriVar(DrawAttribution::EFontFamily,
                                       m_fontFamilyBefore);
            m_page->setDefaultAttriVar(DrawAttribution::EFontWeightStyle,
                                       m_fontWeightBefore);
            m_page->setDefaultAttriVar(DrawAttribution::EFontSize,
                                       m_fontSizeBefore);
        }
        PageStateGuard::TearDown();
    }

    // Returns an index different from the combo's current one, or -1 when
    // the combo has fewer than two entries.
    static int otherIndex(const QComboBox *combo)
    {
        if (combo->count() < 2)
            return -1;
        return combo->currentIndex() == 0 ? 1 : 0;
    }

    Page *m_page = nullptr;
    QVariant m_fontFamilyBefore;
    QVariant m_fontWeightBefore;
    QVariant m_fontSizeBefore;
};

} // namespace

// ===========================================================================
// initFontFamilyWidget — currentIndexChanged lambdas
// ===========================================================================

TEST_F(CTextToolFontLambdaTest, CurrentIndexChanged_SetsActivePackup)
{
    CTextTool *tool = createTextToolForFontTest();
    ASSERT_NE(tool, nullptr);
    ASSERT_NE(tool->m_fontHeavy, nullptr);
    ASSERT_NE(tool->m_fontComBox, nullptr);
    ASSERT_NE(tool->m_fontSize, nullptr);

    const int target = otherIndex(tool->m_fontComBox);
    ASSERT_NE(target, -1) << "font family combo needs at least two entries";

    // The first currentIndexChanged lambda sets _activePackup = true.
    tool->_activePackup = false;
    tool->m_fontComBox->setCurrentIndex(target);
    qApp->processEvents();

    EXPECT_EQ(tool->m_fontComBox->currentIndex(), target);
    EXPECT_TRUE(tool->_activePackup);

    releaseTextTool(tool);
}

TEST_F(CTextToolFontLambdaTest, FontViewShowOut_TriggersPreviewAttribution)
{
    CTextTool *tool = createTextToolForFontTest();
    ASSERT_NE(tool, nullptr);
    ASSERT_NE(tool->m_fontComBox, nullptr);

    const int target = otherIndex(tool->m_fontComBox);
    ASSERT_NE(target, -1) << "font family combo needs at least two entries";
    const QString expectedFamily = tool->m_fontComBox->itemText(target);

    // With _fontViewShowOut set, the second lambda previews the family via
    // drawBoard()->setDrawAttribution(EFontFamily, family, EChanged, false),
    // which stores it as the page's default attribute.
    tool->_fontViewShowOut = true;
    tool->m_fontComBox->setCurrentIndex(target);
    qApp->processEvents();

    EXPECT_EQ(m_page->defaultAttriVar(DrawAttribution::EFontFamily).toString(),
              expectedFamily);

    releaseTextTool(tool);
}

TEST_F(CTextToolFontLambdaTest, FontViewShowOutFalse_NoPreview)
{
    CTextTool *tool = createTextToolForFontTest();
    ASSERT_NE(tool, nullptr);
    ASSERT_NE(tool->m_fontComBox, nullptr);

    const int target = otherIndex(tool->m_fontComBox);
    ASSERT_NE(target, -1) << "font family combo needs at least two entries";
    const QVariant familyBefore =
            m_page->defaultAttriVar(DrawAttribution::EFontFamily);

    // _fontViewShowOut is false — the second lambda skips the preview, so the
    // page default must stay untouched even though the index changes.
    tool->_fontViewShowOut = false;
    tool->m_fontComBox->setCurrentIndex(target);
    qApp->processEvents();

    EXPECT_EQ(tool->m_fontComBox->currentIndex(), target);
    EXPECT_EQ(m_page->defaultAttriVar(DrawAttribution::EFontFamily),
              familyBefore);

    releaseTextTool(tool);
}

// ===========================================================================
// initFontWeightWidget — currentTextChanged lambda
// ===========================================================================

TEST_F(CTextToolFontLambdaTest, CurrentTextChanged_TriggersAttribution)
{
    CTextTool *tool = createTextToolForFontTest();
    ASSERT_NE(tool, nullptr);
    ASSERT_NE(tool->m_fontHeavy, nullptr);

    const int target = otherIndex(tool->m_fontHeavy);
    if (target == -1) {
        releaseTextTool(tool);
        GTEST_SKIP() << "font weight combo has fewer than two entries for the "
                        "default font family";
    }
    const QString expectedStyle = tool->m_fontHeavy->itemText(target);

    // The currentTextChanged lambda forwards the style to
    // drawBoard()->setDrawAttribution(EFontWeightStyle, ...).
    tool->m_fontHeavy->setCurrentIndex(target);
    qApp->processEvents();

    EXPECT_EQ(tool->m_fontHeavy->currentText(), expectedStyle);
    EXPECT_EQ(m_page->defaultAttriVar(DrawAttribution::EFontWeightStyle)
              .toString(), expectedStyle);

    releaseTextTool(tool);
}

TEST_F(CTextToolFontLambdaTest, CurrentTextChanged_SameValue_NoCrash)
{
    CTextTool *tool = createTextToolForFontTest();
    ASSERT_NE(tool, nullptr);
    ASSERT_NE(tool->m_fontHeavy, nullptr);

    const QString curText = tool->m_fontHeavy->currentText();
    const QVariant weightBefore =
            m_page->defaultAttriVar(DrawAttribution::EFontWeightStyle);

    // Re-selecting the same text emits no currentTextChanged signal, so the
    // page default attribute must stay as it was.
    tool->m_fontHeavy->setCurrentText(curText);
    qApp->processEvents();

    EXPECT_EQ(tool->m_fontHeavy->currentText(), curText);
    EXPECT_EQ(m_page->defaultAttriVar(DrawAttribution::EFontWeightStyle),
              weightBefore);

    releaseTextTool(tool);
}

// ===========================================================================
// initFontFontSizeWidget — currentIndexChanged & editingFinished lambdas
// ===========================================================================

TEST_F(CTextToolFontLambdaTest, CurrentIndexChanged_UpdatesFontSize)
{
    CTextTool *tool = createTextToolForFontTest();
    ASSERT_NE(tool, nullptr);
    ASSERT_NE(tool->m_fontSize, nullptr);
    ASSERT_GT(tool->m_fontSize->count(), kSizeIndexTwelve);
    ASSERT_EQ(tool->m_fontSize->itemText(kSizeIndexTwelve),
              QStringLiteral("12px"));

    // Selecting "12px" runs onSizeChanged(12, false).
    tool->m_fontSize->setCurrentIndex(kSizeIndexTwelve);
    qApp->processEvents();

    EXPECT_EQ(tool->_currenFontSize, kFontSizeTwelve);
    EXPECT_EQ(m_page->defaultAttriVar(DrawAttribution::EFontSize).toInt(),
              kFontSizeTwelve);
    EXPECT_EQ(tool->m_fontSize->lineEdit()->text(), QStringLiteral("12px"));

    releaseTextTool(tool);
}

TEST_F(CTextToolFontLambdaTest, EditingFinished_TriggersOnSizeChanged)
{
    CTextTool *tool = createTextToolForFontTest();
    ASSERT_NE(tool, nullptr);
    ASSERT_NE(tool->m_fontSize, nullptr);
    ASSERT_NE(tool->m_fontSize->lineEdit(), nullptr);

    // The editingFinished lambda parses currentText() and calls
    // onSizeChanged(size).
    tool->m_fontSize->lineEdit()->setText(QStringLiteral("18px"));
    emit tool->m_fontSize->lineEdit()->editingFinished();
    qApp->processEvents();

    EXPECT_EQ(tool->_currenFontSize, kFontSizeEighteen);
    EXPECT_EQ(m_page->defaultAttriVar(DrawAttribution::EFontSize).toInt(),
              kFontSizeEighteen);

    releaseTextTool(tool);
}

TEST_F(CTextToolFontLambdaTest, EditingFinished_InvalidValue_ClampedToMinimum)
{
    CTextTool *tool = createTextToolForFontTest();
    ASSERT_NE(tool, nullptr);
    ASSERT_NE(tool->m_fontSize, nullptr);
    ASSERT_NE(tool->m_fontSize->lineEdit(), nullptr);

    // "abc" -> toInt() == 0 -> clamped up to the minimum (8).
    tool->m_fontSize->lineEdit()->setText(QStringLiteral("abc"));
    emit tool->m_fontSize->lineEdit()->editingFinished();
    qApp->processEvents();

    EXPECT_EQ(tool->_currenFontSize, kMinFontSize);
    EXPECT_EQ(m_page->defaultAttriVar(DrawAttribution::EFontSize).toInt(),
              kMinFontSize);

    releaseTextTool(tool);
}

TEST_F(CTextToolFontLambdaTest, EditingFinished_OversizedValue_ClampedToMaximum)
{
    CTextTool *tool = createTextToolForFontTest();
    ASSERT_NE(tool, nullptr);
    ASSERT_NE(tool->m_fontSize, nullptr);
    ASSERT_NE(tool->m_fontSize->lineEdit(), nullptr);

    // A value above 500 is clamped down to the maximum.
    tool->m_fontSize->lineEdit()->setText(QStringLiteral("9999px"));
    emit tool->m_fontSize->lineEdit()->editingFinished();
    qApp->processEvents();

    EXPECT_EQ(tool->_currenFontSize, kMaxFontSize);

    releaseTextTool(tool);
}
