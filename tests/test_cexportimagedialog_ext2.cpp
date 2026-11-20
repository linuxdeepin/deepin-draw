// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Extended unit tests for CExportImageDialog::CExportImageDialog_private
 * (issue V-9047 supplement #4)
 *
 * Target methods:
 *   - autoKeepWHRadio (static) — full branch matrix
 *   - autoKeepSize(EKeepBaseRadioValue) — alert sub-branches
 *   - showTip — ETooSmall/ETooBig text, QTimer first-create + timeout clear
 *
 * Source: src/widgets/dialog/cexportimagedialog.cpp
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QFormLayout>
#include <QWidget>
#include <QTest>
#include <QTimer>

#define protected public
#define private public
#include "cexportimagedialog.h"
#include "cexportimagedialog_p.h"
#include "widgets/cspinbox.h"
#undef protected
#undef private

using Pri = CExportImageDialog::CExportImageDialog_private;
using EKeepBase = CExportImageDialog::CExportImageDialog_private::EKeepBase;
using EAlertReason = CExportImageDialog::CExportImageDialog_private::EAlertReason;

// Helper: create an initialised dialog private object.
static Pri *makeInitedPrivate(QWidget *&contentOwner)
{
    auto *dlg = new CExportImageDialog();
    auto *pri = dlg->_pPrivate;
    auto *content = new QWidget();
    auto *layout = new QFormLayout(content);
    pri->initSizeSettingLayoutUi(layout, content);
    contentOwner = content;
    // Keep the dialog alive so tests can access pri; caller deletes via dlg.
    // Stash content on dlg so it is cleaned up with the dialog.
    content->setParent(dlg);
    return pri;
}

// =========================================================================
// autoKeepWHRadio — static, EKeepBaseRadioValue branch
// =========================================================================

TEST(ExportImageDialogExt2, RadioValueValidScaling)
{
    EAlertReason aler = EAlertReason::ENoAlert;
    QSizeF preSize(100, 100);
    QSize orgSz(100, 100);
    QSizeF result = Pri::autoKeepWHRadio(EKeepBase::EKeepBaseRadioValue, 0.5,
                                         QSize(50, 50), preSize, orgSz, aler);
    EXPECT_EQ(aler, EAlertReason::ENoAlert);
    EXPECT_NEAR(result.width(), 50.0, 0.01);
    EXPECT_NEAR(result.height(), 50.0, 0.01);
}

TEST(ExportImageDialogExt2, RadioValueOverMaxFallsBack)
{
    EAlertReason aler = EAlertReason::ENoAlert;
    QSizeF preSize(100, 100);
    QSize orgSz(10000, 10000);
    // 10000 * 2.0 = 20000 > 10000 → ETooBig, returns preSize
    QSizeF result = Pri::autoKeepWHRadio(EKeepBase::EKeepBaseRadioValue, 2.0,
                                         QSize(20000, 20000), preSize, orgSz, aler);
    EXPECT_EQ(aler, EAlertReason::ETooBig);
    EXPECT_NEAR(result.width(), 100.0, 0.01);
    EXPECT_NEAR(result.height(), 100.0, 0.01);
}

TEST(ExportImageDialogExt2, RadioValueBelowMinFallsBack)
{
    EAlertReason aler = EAlertReason::ENoAlert;
    QSizeF preSize(1, 1);
    QSize orgSz(1, 1);
    // 1 * 0.5 = 0.5 < 1 → ETooSmall, returns preSize
    QSizeF result = Pri::autoKeepWHRadio(EKeepBase::EKeepBaseRadioValue, 0.5,
                                         QSize(1, 1), preSize, orgSz, aler);
    EXPECT_EQ(aler, EAlertReason::ETooSmall);
    EXPECT_NEAR(result.width(), 1.0, 0.01);
    EXPECT_NEAR(result.height(), 1.0, 0.01);
}

// =========================================================================
// autoKeepWHRadio — EKeepBaseW branch
// =========================================================================

TEST(ExportImageDialogExt2, KeepBaseWResultInRange)
{
    EAlertReason aler = EAlertReason::ENoAlert;
    // radioWH = preSize.w / preSize.h = 100/100 = 1.0
    // resultH = size.width() / radioWH = 200 / 1.0 = 200, in [1,10000]
    QSizeF preSize(100, 100);
    QSizeF result = Pri::autoKeepWHRadio(EKeepBase::EKeepBaseW, 1.0,
                                         QSize(200, 100), preSize, QSize(100, 100), aler);
    EXPECT_EQ(aler, EAlertReason::ENoAlert);
    EXPECT_NEAR(result.width(), 200.0, 0.01);
    EXPECT_NEAR(result.height(), 200.0, 0.01);
}

TEST(ExportImageDialogExt2, KeepBaseWResultOverMax)
{
    EAlertReason aler = EAlertReason::ENoAlert;
    // radioWH = 1/10000 = 0.0001
    // resultH = 10000 / 0.0001 = 1e8 > 10000 → ETooBig
    QSizeF preSize(1, 10000);
    QSizeF result = Pri::autoKeepWHRadio(EKeepBase::EKeepBaseW, 1.0,
                                         QSize(10000, 1), preSize, QSize(1, 10000), aler);
    EXPECT_EQ(aler, EAlertReason::ETooBig);
    // Falls back to preSize
    EXPECT_NEAR(result.width(), 1.0, 0.01);
    EXPECT_NEAR(result.height(), 10000.0, 0.01);
}

TEST(ExportImageDialogExt2, KeepBaseWResultBelowMin)
{
    EAlertReason aler = EAlertReason::ENoAlert;
    // radioWH = 10000/1 = 10000
    // resultH = 1 / 10000 = 0.0001 < 1 → ETooSmall
    QSizeF preSize(10000, 1);
    QSizeF result = Pri::autoKeepWHRadio(EKeepBase::EKeepBaseW, 1.0,
                                         QSize(1, 10000), preSize, QSize(10000, 1), aler);
    EXPECT_EQ(aler, EAlertReason::ETooSmall);
    EXPECT_NEAR(result.width(), 10000.0, 0.01);
    EXPECT_NEAR(result.height(), 1.0, 0.01);
}

// =========================================================================
// autoKeepWHRadio — EKeepBaseH branch (symmetric)
// =========================================================================

TEST(ExportImageDialogExt2, KeepBaseHResultInRange)
{
    EAlertReason aler = EAlertReason::ENoAlert;
    // radioWH = 100/100 = 1.0
    // resultW = size.height() * radioWH = 200 * 1.0 = 200, in [1,10000]
    QSizeF preSize(100, 100);
    QSizeF result = Pri::autoKeepWHRadio(EKeepBase::EKeepBaseH, 1.0,
                                         QSize(100, 200), preSize, QSize(100, 100), aler);
    EXPECT_EQ(aler, EAlertReason::ENoAlert);
    EXPECT_NEAR(result.width(), 200.0, 0.01);
    EXPECT_NEAR(result.height(), 200.0, 0.01);
}

TEST(ExportImageDialogExt2, KeepBaseHResultOverMax)
{
    EAlertReason aler = EAlertReason::ENoAlert;
    // radioWH = 10000/1 = 10000
    // resultW = size.height() * radioWH = 10000 * 10000 = 1e8 > 10000 → ETooBig
    QSizeF preSize(10000, 1);
    QSizeF result = Pri::autoKeepWHRadio(EKeepBase::EKeepBaseH, 1.0,
                                         QSize(1, 10000), preSize, QSize(10000, 1), aler);
    EXPECT_EQ(aler, EAlertReason::ETooBig);
    EXPECT_NEAR(result.width(), 10000.0, 0.01);
    EXPECT_NEAR(result.height(), 1.0, 0.01);
}

TEST(ExportImageDialogExt2, KeepBaseHResultBelowMin)
{
    EAlertReason aler = EAlertReason::ENoAlert;
    // radioWH = 1/10000 = 0.0001
    // resultW = size.height() * radioWH = 1 * 0.0001 = 0.0001 < 1 → ETooSmall
    QSizeF preSize(1, 10000);
    QSizeF result = Pri::autoKeepWHRadio(EKeepBase::EKeepBaseH, 1.0,
                                         QSize(10000, 1), preSize, QSize(1, 10000), aler);
    EXPECT_EQ(aler, EAlertReason::ETooSmall);
    EXPECT_NEAR(result.width(), 1.0, 0.01);
    EXPECT_NEAR(result.height(), 10000.0, 0.01);
}

// =========================================================================
// autoKeepWHRadio — EFreeSetting branch (clamp width / height)
// =========================================================================

TEST(ExportImageDialogExt2, FreeSettingWidthOverMax)
{
    EAlertReason aler = EAlertReason::ENoAlert;
    QSizeF preSize(100, 100);
    QSizeF result = Pri::autoKeepWHRadio(EKeepBase::EFreeSetting, 1.0,
                                         QSize(20000, 100), preSize, QSize(100, 100), aler);
    EXPECT_EQ(aler, EAlertReason::ETooBig);
    EXPECT_NEAR(result.width(), 10000.0, 0.01);
    EXPECT_NEAR(result.height(), 100.0, 0.01);
}

TEST(ExportImageDialogExt2, FreeSettingWidthBelowMin)
{
    EAlertReason aler = EAlertReason::ENoAlert;
    QSizeF preSize(100, 100);
    QSizeF result = Pri::autoKeepWHRadio(EKeepBase::EFreeSetting, 1.0,
                                         QSize(0, 100), preSize, QSize(100, 100), aler);
    EXPECT_EQ(aler, EAlertReason::ETooSmall);
    EXPECT_NEAR(result.width(), 1.0, 0.01);
    EXPECT_NEAR(result.height(), 100.0, 0.01);
}

TEST(ExportImageDialogExt2, FreeSettingHeightOverMax)
{
    EAlertReason aler = EAlertReason::ENoAlert;
    QSizeF preSize(100, 100);
    QSizeF result = Pri::autoKeepWHRadio(EKeepBase::EFreeSetting, 1.0,
                                         QSize(100, 20000), preSize, QSize(100, 100), aler);
    EXPECT_EQ(aler, EAlertReason::ETooBig);
    EXPECT_NEAR(result.width(), 100.0, 0.01);
    EXPECT_NEAR(result.height(), 10000.0, 0.01);
}

TEST(ExportImageDialogExt2, FreeSettingHeightBelowMin)
{
    EAlertReason aler = EAlertReason::ENoAlert;
    QSizeF preSize(100, 100);
    QSizeF result = Pri::autoKeepWHRadio(EKeepBase::EFreeSetting, 1.0,
                                         QSize(100, 0), preSize, QSize(100, 100), aler);
    EXPECT_EQ(aler, EAlertReason::ETooSmall);
    EXPECT_NEAR(result.width(), 100.0, 0.01);
    EXPECT_NEAR(result.height(), 1.0, 0.01);
}

TEST(ExportImageDialogExt2, FreeSettingBothInRange)
{
    EAlertReason aler = EAlertReason::ENoAlert;
    QSizeF preSize(100, 100);
    QSizeF result = Pri::autoKeepWHRadio(EKeepBase::EFreeSetting, 1.0,
                                         QSize(500, 300), preSize, QSize(100, 100), aler);
    EXPECT_EQ(aler, EAlertReason::ENoAlert);
    EXPECT_NEAR(result.width(), 500.0, 0.01);
    EXPECT_NEAR(result.height(), 300.0, 0.01);
}

// =========================================================================
// autoKeepSize(EKeepBaseRadioValue) — alert sub-branches
// =========================================================================

TEST(ExportImageDialogExt2, AutoKeepSizeRadioValueNoAlertUpdatesCurPrecent)
{
    QWidget *content = nullptr;
    auto *dlg = new CExportImageDialog();
    auto *pri = dlg->_pPrivate;
    QWidget *c = new QWidget();
    QFormLayout *fl = new QFormLayout(c);
    pri->initSizeSettingLayoutUi(fl, c);
    c->setParent(dlg);

    // Configure for a valid radio scaling: orgSz=100x100, precent=100% → 100x100 (in range)
    // Use default spinbox value (100 = 1.0x). Block signals to prevent autoKeepSize recursion.
    pri->settingModel = Pri::ERadioModel;
    pri->originSize = QSize(100, 100);
    pri->curPrecent = 0.5;  // different from settingPrecent to verify update
    pri->_radioSpinBox->blockSignals(true);
    pri->_radioSpinBox->setValue(100);  // precent = 1.0
    pri->_radioSpinBox->blockSignals(false);
    pri->curSize[Pri::ERadioModel] = QSizeF(100, 100);
    pri->_widthEditor->setText("100");
    pri->_heightEditor->setText("100");

    bool ok = pri->autoKeepSize(EKeepBase::EKeepBaseRadioValue);
    // No alert → returns true, curPrecent updated to settingPrecent (1.0)
    EXPECT_TRUE(ok);
    EXPECT_NEAR(pri->curPrecent, 1.0, 0.01);

    delete dlg;
}

TEST(ExportImageDialogExt2, AutoKeepSizeRadioValueAlertRestoresSpinBox)
{
    QWidget *content = nullptr;
    auto *dlg = new CExportImageDialog();
    auto *pri = dlg->_pPrivate;
    QWidget *c = new QWidget();
    QFormLayout *fl = new QFormLayout(c);
    pri->initSizeSettingLayoutUi(fl, c);
    c->setParent(dlg);

    // Configure for an over-max scaling: orgSz=20000x20000, precent=100% → 20000x20000 (>10000)
    // Use default spinbox value (100 = 1.0x). Block signals to prevent autoKeepSize recursion.
    pri->settingModel = Pri::ERadioModel;
    pri->originSize = QSize(20000, 20000);
    pri->curPrecent = 1.0;
    pri->_radioSpinBox->blockSignals(true);
    pri->_radioSpinBox->setValue(100);  // precent = 1.0
    pri->_radioSpinBox->blockSignals(false);
    pri->curSize[Pri::ERadioModel] = QSizeF(100, 100);
    pri->_widthEditor->setText("20000");
    pri->_heightEditor->setText("20000");

    bool ok = pri->autoKeepSize(EKeepBase::EKeepBaseRadioValue);
    // Alert (ETooBig) → returns false; curPrecent not updated (stays 1.0)
    EXPECT_FALSE(ok);
    EXPECT_NEAR(pri->curPrecent, 1.0, 0.01);

    delete dlg;
}

// =========================================================================
// showTip — ETooSmall / ETooBig text + QTimer lifecycle
// =========================================================================

TEST(ExportImageDialogExt2, ShowTipTooSmallSetsText)
{
    auto *dlg = new CExportImageDialog();
    auto *pri = dlg->_pPrivate;
    QWidget *c = new QWidget();
    QFormLayout *fl = new QFormLayout(c);
    pri->initSizeSettingLayoutUi(fl, c);
    c->setParent(dlg);

    pri->timer = nullptr;  // ensure fresh
    pri->showTip(EAlertReason::ETooSmall);
    EXPECT_FALSE(pri->_tipLabelForOutOfBounds->text().isEmpty());
    // Timer should be created on first call
    EXPECT_NE(pri->timer, nullptr);
    EXPECT_TRUE(pri->timer->isSingleShot());

    delete dlg;
}

TEST(ExportImageDialogExt2, ShowTipTooBigSetsText)
{
    auto *dlg = new CExportImageDialog();
    auto *pri = dlg->_pPrivate;
    QWidget *c = new QWidget();
    QFormLayout *fl = new QFormLayout(c);
    pri->initSizeSettingLayoutUi(fl, c);
    c->setParent(dlg);

    pri->timer = nullptr;
    pri->showTip(EAlertReason::ETooBig);
    EXPECT_FALSE(pri->_tipLabelForOutOfBounds->text().isEmpty());
    EXPECT_NE(pri->timer, nullptr);

    delete dlg;
}

TEST(ExportImageDialogExt2, ShowTipNoAlertReturnsEarly)
{
    auto *dlg = new CExportImageDialog();
    auto *pri = dlg->_pPrivate;
    QWidget *c = new QWidget();
    QFormLayout *fl = new QFormLayout(c);
    pri->initSizeSettingLayoutUi(fl, c);
    c->setParent(dlg);

    pri->timer = nullptr;
    pri->_tipLabelForOutOfBounds->setText("preexisting");
    pri->showTip(EAlertReason::ENoAlert);
    // Should return early without changing text or creating timer
    EXPECT_EQ(pri->timer, nullptr);
    EXPECT_EQ(pri->_tipLabelForOutOfBounds->text().toStdString(), "preexisting");

    delete dlg;
}

TEST(ExportImageDialogExt2, ShowTipTimerClearsAfter2s)
{
    auto *dlg = new CExportImageDialog();
    auto *pri = dlg->_pPrivate;
    QWidget *c = new QWidget();
    QFormLayout *fl = new QFormLayout(c);
    pri->initSizeSettingLayoutUi(fl, c);
    c->setParent(dlg);

    pri->timer = nullptr;
    pri->showTip(EAlertReason::ETooSmall);
    EXPECT_FALSE(pri->_tipLabelForOutOfBounds->text().isEmpty());
    // Wait for the 2-second single-shot timer to fire
    QTest::qWait(2200);
    EXPECT_TRUE(pri->_tipLabelForOutOfBounds->text().isEmpty());

    delete dlg;
}

TEST(ExportImageDialogExt2, ShowTipReusesExistingTimer)
{
    auto *dlg = new CExportImageDialog();
    auto *pri = dlg->_pPrivate;
    QWidget *c = new QWidget();
    QFormLayout *fl = new QFormLayout(c);
    pri->initSizeSettingLayoutUi(fl, c);
    c->setParent(dlg);

    pri->timer = nullptr;
    pri->showTip(EAlertReason::ETooSmall);
    QTimer *firstTimer = pri->timer;
    ASSERT_NE(firstTimer, nullptr);

    // Second call should reuse the same timer
    pri->showTip(EAlertReason::ETooBig);
    EXPECT_EQ(pri->timer, firstTimer);

    delete dlg;
}


