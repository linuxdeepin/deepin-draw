// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for FileSelectDialog and PathActiveButton (issue V-7155)
 *
 * Source: src/frame/cmultiptabbarwidget.h/.cpp
 *         src/widgets/dialog/cexportimagedialog.h
 *
 * Branch map:
 *   B1  FileSelectDialog constructor with nullptr parent (no crash)
 *   B2  borad() returns nullptr when parent is nullptr
 *   B3  resultFile() returns empty QString initially
 *   B4  extractSuffix("Images (*.png *.jpg)") → "png"
 *   B5  extractSuffix("Doc (*.pdf)") → "pdf"
 *   B6  extractSuffix("All Files (*)") → "" (no wildcard-prefix match)
 *   B7  extractSuffix("NoBrackets") → "" (no bracket match)
 *   B8  extractSuffix("") → "" (empty input)
 *   B9  exec() skipped (native dialog)
 *   B10 PathActiveButton constructor (no crash)
 *   B11 PathActiveButton paintEvent via grab()
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QString>

#include "cmultiptabbarwidget.h"
#include "cexportimagedialog.h"

#if TEST_FILEDIALOG_FUNCS_ITEM

// =========================================================================
// FileSelectDialog
// =========================================================================

TEST(FileSelectDialogTest, Constructor_NullptrParent_NoCrash)
{
    // B1: constructor with nullptr parent
    FileSelectDialog dialog(nullptr);
    EXPECT_EQ(dialog.acceptMode(), QFileDialog::AcceptSave);
    SUCCEED();
}

TEST(FileSelectDialogTest, Borad_ReturnsNullptr_WhenParentIsNullptr)
{
    // B2: borad() returns nullptr when parent is nullptr
    FileSelectDialog dialog(nullptr);
    EXPECT_EQ(dialog.borad(), nullptr);
}

TEST(FileSelectDialogTest, ResultFile_InitiallyEmpty)
{
    // B3: resultFile() returns empty QString initially
    FileSelectDialog dialog(nullptr);
    EXPECT_TRUE(dialog.resultFile().isEmpty());
}

TEST(FileSelectDialogTest, ExtractSuffix_MultipleSuffixes_ReturnsFirst)
{
    // B4: extractSuffix("Images (*.png *.jpg)") → "png"
    FileSelectDialog dialog(nullptr);
    EXPECT_EQ(dialog.extractSuffix("Images (*.png *.jpg)"), QString("png"));
}

TEST(FileSelectDialogTest, ExtractSuffix_SingleSuffix_ReturnsSuffix)
{
    // B5: extractSuffix("Doc (*.pdf)") → "pdf"
    FileSelectDialog dialog(nullptr);
    EXPECT_EQ(dialog.extractSuffix("Doc (*.pdf)"), QString("pdf"));
}

TEST(FileSelectDialogTest, ExtractSuffix_BareWildcard_ReturnsEmpty)
{
    // B6: extractSuffix("All Files (*)") → "" (no "*." prefix)
    FileSelectDialog dialog(nullptr);
    EXPECT_EQ(dialog.extractSuffix("All Files (*)"), QString());
}

TEST(FileSelectDialogTest, ExtractSuffix_NoBrackets_ReturnsEmpty)
{
    // B7: extractSuffix("NoBrackets") → "" (no bracket match)
    FileSelectDialog dialog(nullptr);
    EXPECT_EQ(dialog.extractSuffix("NoBrackets"), QString());
}

TEST(FileSelectDialogTest, ExtractSuffix_EmptyInput_ReturnsEmpty)
{
    // B8: extractSuffix("") → ""
    FileSelectDialog dialog(nullptr);
    EXPECT_EQ(dialog.extractSuffix(""), QString());
}

TEST(FileSelectDialogTest, Exec_Skipped_NativeDialog)
{
    // B9: exec() involves native dialog — skip in offscreen test env
    GTEST_SKIP() << "exec() launches a native file dialog, not suitable for unit tests";
}

// =========================================================================
// PathActiveButton
// =========================================================================

TEST(PathActiveButtonTest, Constructor_NoCrash)
{
    // B10: constructor (inherited from DSuggestButton)
    PathActiveButton btn(nullptr);
    SUCCEED();
}

TEST(PathActiveButtonTest, PaintEvent_ViaGrab_NoCrash)
{
    // B11: paintEvent via grab()
    PathActiveButton btn(nullptr);
    btn.resize(100, 30);
    QPixmap pix = btn.grab();
    EXPECT_FALSE(pix.isNull());
}

#endif // TEST_FILEDIALOG_FUNCS_ITEM
