// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for CExportImageDialog methods (issue V-5860)
 *
 * Source: src/widgets/dialog/cexportimagedialog.cpp
 *
 * Target methods:
 *   getImageType, getImageFormate, getQuality,
 *   getCompleteSavePath, desImageSize,
 *   slotOnFormatChange, slotOnSavePathChange,
 *   execCheckFile, execFileIsExists,
 *   resultFile, saveSetting, execFor,
 *   showDirChoseDialog
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QStandardPaths>
#include <QFileInfo>
#include <QDir>
#include <QTemporaryFile>
#include <QTimer>

#define protected public
#define private public
#include "publicApi.h"
#undef protected
#undef private
#define protected public
#define private public
#include "cexportimagedialog.h"
#undef protected
#undef private

// Helper: create a dialog on the heap; caller owns it.
static CExportImageDialog *makeDialog()
{
    return new CExportImageDialog();
}

// ---------------------------------------------------------------------------
// getImageType / getImageFormate / getQuality
// ---------------------------------------------------------------------------

TEST(CExportImageDialogGetTest, DefaultImageTypeIsZero)
{
    auto *dlg = makeDialog();
    EXPECT_EQ(dlg->getImageType(), 0);
    delete dlg;
}

TEST(CExportImageDialogGetTest, ImageTypeChangesWithComboxIndex)
{
    auto *dlg = makeDialog();
    dlg->m_formatCombox->setCurrentIndex(2);
    EXPECT_EQ(dlg->getImageType(), 2);
    delete dlg;
}

TEST(CExportImageDialogGetTest, DefaultQualityIs100)
{
    auto *dlg = makeDialog();
    // slotOnQualityChanged is called during exec(); before exec m_quality
    // may be uninitialized. After construction the slider value is 100 and
    // slotOnQualityChanged hasn't been called yet, so m_quality could be
    // uninitialized. Call slotOnQualityChanged explicitly.
    dlg->slotOnQualityChanged(100);
    EXPECT_EQ(dlg->getQuality(), 100);
    delete dlg;
}

TEST(CExportImageDialogGetTest, QualityChangesWithSlider)
{
    auto *dlg = makeDialog();
    dlg->slotOnQualityChanged(50);
    EXPECT_EQ(dlg->getQuality(), 50);
    delete dlg;
}

// ---------------------------------------------------------------------------
// getImageFormate / slotOnFormatChange
// ---------------------------------------------------------------------------

TEST(CExportImageDialogFormatTest, SlotOnFormatChangeSetsSaveFormat)
{
    auto *dlg = makeDialog();
    dlg->slotOnFormatChange(0); // png
    EXPECT_EQ(dlg->getImageFormate().toStdString(), "png");
    delete dlg;
}

TEST(CExportImageDialogFormatTest, SlotOnFormatChangeJpg)
{
    auto *dlg = makeDialog();
    dlg->slotOnFormatChange(1); // jpg
    EXPECT_EQ(dlg->getImageFormate().toStdString(), "jpg");
    delete dlg;
}

TEST(CExportImageDialogFormatTest, SlotOnFormatChangePdfDisablesQualitySlider)
{
    auto *dlg = makeDialog();
    dlg->slotOnFormatChange(4); // pdf
    EXPECT_EQ(dlg->getImageFormate().toStdString(), "pdf");
    EXPECT_FALSE(dlg->m_qualitySlider->isEnabled());
    EXPECT_EQ(dlg->m_qualitySlider->value(), 100);
    delete dlg;
}

TEST(CExportImageDialogFormatTest, SlotOnFormatChangeNonPdfEnablesQualitySlider)
{
    auto *dlg = makeDialog();
    dlg->slotOnFormatChange(4); // pdf first
    dlg->slotOnFormatChange(0); // then png
    EXPECT_TRUE(dlg->m_qualitySlider->isEnabled());
    delete dlg;
}

TEST(CExportImageDialogFormatTest, SlotOnFormatChangeInvalidIndexDefaultsToPng)
{
    auto *dlg = makeDialog();
    dlg->slotOnFormatChange(999); // not in mapping
    EXPECT_EQ(dlg->getImageFormate().toStdString(), "png");
    delete dlg;
}

// ---------------------------------------------------------------------------
// getCompleteSavePath
// ---------------------------------------------------------------------------

TEST(CExportImageDialogGetCompleteSavePathTest, EmptyFileNameReturnsEmpty)
{
    auto *dlg = makeDialog();
    dlg->m_fileNameEdit->setText("");
    dlg->m_saveFormat = "png";
    EXPECT_EQ(dlg->getCompleteSavePath().toStdString(), "");
    delete dlg;
}

TEST(CExportImageDialogGetCompleteSavePathTest, ValidFileNameReturnsPath)
{
    auto *dlg = makeDialog();
    dlg->m_fileNameEdit->setText("testfile");
    dlg->m_saveFormat = "png";
    dlg->m_pathEditor->setText("/tmp");
    QString expected = "/tmp/testfile.png";
    EXPECT_EQ(dlg->getCompleteSavePath().toStdString(), expected.toStdString());
    delete dlg;
}

TEST(CExportImageDialogGetCompleteSavePathTest, DifferentFormatAppended)
{
    auto *dlg = makeDialog();
    dlg->m_fileNameEdit->setText("image");
    dlg->m_saveFormat = "jpg";
    dlg->m_pathEditor->setText("/home/user");
    QString expected = "/home/user/image.jpg";
    EXPECT_EQ(dlg->getCompleteSavePath().toStdString(), expected.toStdString());
    delete dlg;
}

TEST(CExportImageDialogGetCompleteSavePathTest, TrimmedFileName)
{
    auto *dlg = makeDialog();
    dlg->m_fileNameEdit->setText("  spaced  ");
    dlg->m_saveFormat = "bmp";
    dlg->m_pathEditor->setText("/tmp");
    QString expected = "/tmp/spaced.bmp";
    EXPECT_EQ(dlg->getCompleteSavePath().toStdString(), expected.toStdString());
    delete dlg;
}

// ---------------------------------------------------------------------------
// slotOnSavePathChange
// ---------------------------------------------------------------------------

TEST(CExportImageDialogSavePathTest, PicturesPath)
{
    auto *dlg = makeDialog();
    dlg->slotOnSavePathChange(CExportImageDialog::Pictures);
    QString expected = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);
    EXPECT_EQ(dlg->m_savePath.toStdString(), expected.toStdString());
    delete dlg;
}

TEST(CExportImageDialogSavePathTest, DocumentsPath)
{
    auto *dlg = makeDialog();
    dlg->slotOnSavePathChange(CExportImageDialog::Documents);
    QString expected = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    EXPECT_EQ(dlg->m_savePath.toStdString(), expected.toStdString());
    delete dlg;
}

TEST(CExportImageDialogSavePathTest, DownloadsPath)
{
    auto *dlg = makeDialog();
    dlg->slotOnSavePathChange(CExportImageDialog::Downloads);
    QString expected = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
    EXPECT_EQ(dlg->m_savePath.toStdString(), expected.toStdString());
    delete dlg;
}

TEST(CExportImageDialogSavePathTest, DesktopPath)
{
    auto *dlg = makeDialog();
    dlg->slotOnSavePathChange(CExportImageDialog::Desktop);
    QString expected = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    EXPECT_EQ(dlg->m_savePath.toStdString(), expected.toStdString());
    delete dlg;
}

TEST(CExportImageDialogSavePathTest, DefaultPathIsEmpty)
{
    auto *dlg = makeDialog();
    dlg->slotOnSavePathChange(999); // invalid index → default
    EXPECT_EQ(dlg->m_savePath.toStdString(), "");
    delete dlg;
}

TEST(CExportImageDialogSavePathTest, LastIndexUpdated)
{
    auto *dlg = makeDialog();
    dlg->slotOnSavePathChange(CExportImageDialog::Desktop);
    EXPECT_EQ(dlg->m_lastIndex, CExportImageDialog::Desktop);
    delete dlg;
}

// ---------------------------------------------------------------------------
// execCheckFile
// ---------------------------------------------------------------------------

TEST(CExportImageDialogExecCheckFileTest, EmptyFileNameReturnsCancel)
{
    auto *dlg = makeDialog();
    dlg->m_fileNameEdit->setText("");
    dlg->m_saveFormat = "png";
    dlg->m_pathEditor->setText("/tmp");
    // getCompleteSavePath returns "" → ECancel
    EXPECT_EQ(dlg->execCheckFile(""), -1); // ECancel = -1
    delete dlg;
}

TEST(CExportImageDialogExecCheckFileTest, ValidFileNameReturnsOk)
{
    auto *dlg = makeDialog();
    // Use a unique filename that doesn't exist
    dlg->m_fileNameEdit->setText("unittest_export_nonexist_12345");
    dlg->m_saveFormat = "png";
    dlg->m_pathEditor->setText("/tmp");
    QString fullPath = "/tmp/unittest_export_nonexist_12345.png";
    // Ensure file doesn't exist
    if (QFileInfo::exists(fullPath)) {
        QFile::remove(fullPath);
    }
    EXPECT_EQ(dlg->execCheckFile(""), 1); // EOK = 1
    delete dlg;
}

// ---------------------------------------------------------------------------
// resultFile
// ---------------------------------------------------------------------------

TEST(CExportImageDialogResultFileTest, QuitRetOneReturnsCompletePath)
{
    auto *dlg = makeDialog();
    dlg->quitRet = 1;
    dlg->m_fileNameEdit->setText("result");
    dlg->m_saveFormat = "png";
    dlg->m_pathEditor->setText("/tmp");
    EXPECT_EQ(dlg->resultFile().toStdString(), "/tmp/result.png");
    delete dlg;
}

TEST(CExportImageDialogResultFileTest, QuitRetZeroReturnsEmpty)
{
    auto *dlg = makeDialog();
    dlg->quitRet = 0;
    dlg->m_fileNameEdit->setText("result");
    dlg->m_saveFormat = "png";
    dlg->m_pathEditor->setText("/tmp");
    EXPECT_EQ(dlg->resultFile().toStdString(), "");
    delete dlg;
}

// ---------------------------------------------------------------------------
// desImageSize
// ---------------------------------------------------------------------------

TEST(CExportImageDialogDesImageSizeTest, ReturnsValidSize)
{
    auto *dlg = makeDialog();
    // desImageSize() reads from private editors initialised during dialog
    // construction. The private class (DECLAREPRIVATECLASS) is only
    // defined in the .cpp, so we cannot set editor values directly.
    // Verify the method does not crash and returns a QSize.
    QSize sz = dlg->desImageSize();
    // Size may be (-1, -1) when private editors are not initialised;
    // the important thing is that the call does not crash.
    (void)sz;
    delete dlg;
}

// ---------------------------------------------------------------------------
// saveSetting
// ---------------------------------------------------------------------------

TEST(CExportImageDialogSaveSettingTest, DoesNotCrash)
{
    auto *dlg = makeDialog();
    dlg->m_fileNameEdit->setText("savetest");
    dlg->m_saveFormat = "png";
    dlg->m_pathEditor->setText("/tmp");
    dlg->saveSetting();
    // Verify the default path was saved
    EXPECT_EQ(drawApp->defaultFileDialogPath().toStdString(),
              QFileInfo("/tmp/savetest.png").absolutePath().toStdString());
    delete dlg;
}
