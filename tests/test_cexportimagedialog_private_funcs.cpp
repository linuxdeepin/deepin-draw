// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for CExportImageDialog::CExportImageDialog_private (issue V-7429)
 *
 * Source: src/widgets/dialog/cexportimagedialog.cpp (nested class)
 * Header: src/widgets/dialog/cexportimagedialog_p.h
 *
 * Target methods:
 *   initSizeSettingLayoutUi(QFormLayout*, QWidget*)
 *   updateSettingModelUi()
 *   setSizeSettingModel(ESizeSettingModel)
 *   autoKeepSize(EKeepBase)
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QFormLayout>
#include <QWidget>
#include <QSignalSpy>

#define protected public
#define private public
#include "cexportimagedialog.h"
#include "cexportimagedialog_p.h"
#include "widgets/cspinbox.h"
#undef protected
#undef private

// Helper: create a dialog and access its private object.
static CExportImageDialog *makeDialog2()
{
    return new CExportImageDialog();
}

// =========================================================================
// CExportImageDialog_private::initSizeSettingLayoutUi
// =========================================================================

TEST(CExportImageDialogPrivateInitUi, CreatesWidgetsInLayout)
{
    auto *dlg = makeDialog2();
    auto *pri = dlg->_pPrivate;
    ASSERT_NE(pri, nullptr);

    QWidget *content = new QWidget();
    QFormLayout *layout = new QFormLayout(content);

    pri->initSizeSettingLayoutUi(layout, content);

    // After init, the radio buttons should be created.
    EXPECT_NE(pri->_radioRadioBtn, nullptr);
    EXPECT_NE(pri->_radioPiexlBtn, nullptr);
    EXPECT_NE(pri->_radioSpinBox, nullptr);
    EXPECT_NE(pri->_widthEditor, nullptr);
    EXPECT_NE(pri->_heightEditor, nullptr);
    EXPECT_NE(pri->_keepRaidoCheckBox, nullptr);
    EXPECT_NE(pri->piexlWgt, nullptr);

    delete content;
    delete dlg;
}

TEST(CExportImageDialogPrivateInitUi, RadioBtnCheckedByDefault)
{
    auto *dlg = makeDialog2();
    auto *pri = dlg->_pPrivate;

    QWidget *content = new QWidget();
    QFormLayout *layout = new QFormLayout(content);
    pri->initSizeSettingLayoutUi(layout, content);

    EXPECT_TRUE(pri->_radioRadioBtn->isChecked());
    EXPECT_FALSE(pri->_radioPiexlBtn->isChecked());

    delete content;
    delete dlg;
}

// =========================================================================
// CExportImageDialog_private::setSizeSettingModel / updateSettingModelUi
// =========================================================================

TEST(CExportImageDialogPrivateSetModel, SetRadioModelEnablesSpinBox)
{
    auto *dlg = makeDialog2();
    auto *pri = dlg->_pPrivate;

    QWidget *content = new QWidget();
    QFormLayout *layout = new QFormLayout(content);
    pri->initSizeSettingLayoutUi(layout, content);
    pri->originSize = QSize(100, 100);
    pri->curSize[0] = QSizeF(100, 100);
    pri->curSize[1] = QSizeF(100, 100);

    pri->setSizeSettingModel(CExportImageDialog::CExportImageDialog_private::ERadioModel);

    EXPECT_TRUE(pri->_radioSpinBox->isEnabled());
    EXPECT_FALSE(pri->piexlWgt->isEnabled());

    delete content;
    delete dlg;
}

TEST(CExportImageDialogPrivateSetModel, SetPixelModelDisablesSpinBox)
{
    auto *dlg = makeDialog2();
    auto *pri = dlg->_pPrivate;

    QWidget *content = new QWidget();
    QFormLayout *layout = new QFormLayout(content);
    pri->initSizeSettingLayoutUi(layout, content);
    pri->originSize = QSize(100, 100);
    pri->curSize[0] = QSizeF(100, 100);
    pri->curSize[1] = QSizeF(100, 100);

    pri->setSizeSettingModel(CExportImageDialog::CExportImageDialog_private::EPixelModel);

    EXPECT_FALSE(pri->_radioSpinBox->isEnabled());
    EXPECT_TRUE(pri->piexlWgt->isEnabled());

    delete content;
    delete dlg;
}

TEST(CExportImageDialogPrivateSetModel, UpdateModelUiSwitchesToPixel)
{
    auto *dlg = makeDialog2();
    auto *pri = dlg->_pPrivate;

    QWidget *content = new QWidget();
    QFormLayout *layout = new QFormLayout(content);
    pri->initSizeSettingLayoutUi(layout, content);
    pri->originSize = QSize(100, 100);
    pri->curSize[0] = QSizeF(100, 100);
    pri->curSize[1] = QSizeF(100, 100);

    pri->_radioPiexlBtn->setChecked(true);
    pri->updateSettingModelUi();

    EXPECT_EQ(pri->settingModel, CExportImageDialog::CExportImageDialog_private::EPixelModel);

    delete content;
    delete dlg;
}

TEST(CExportImageDialogPrivateSetModel, UpdateModelUiSwitchesToRadio)
{
    auto *dlg = makeDialog2();
    auto *pri = dlg->_pPrivate;

    QWidget *content = new QWidget();
    QFormLayout *layout = new QFormLayout(content);
    pri->initSizeSettingLayoutUi(layout, content);
    pri->originSize = QSize(100, 100);
    pri->curSize[0] = QSizeF(100, 100);
    pri->curSize[1] = QSizeF(100, 100);

    pri->_radioPiexlBtn->setChecked(true);
    pri->_radioRadioBtn->setChecked(true);
    pri->updateSettingModelUi();

    EXPECT_EQ(pri->settingModel, CExportImageDialog::CExportImageDialog_private::ERadioModel);

    delete content;
    delete dlg;
}

// =========================================================================
// CExportImageDialog_private::autoKeepSize
// =========================================================================

TEST(CExportImageDialogPrivateAutoKeepSize, FreeSettingReturnsTrue)
{
    auto *dlg = makeDialog2();
    auto *pri = dlg->_pPrivate;

    QWidget *content = new QWidget();
    QFormLayout *layout = new QFormLayout(content);
    pri->initSizeSettingLayoutUi(layout, content);
    pri->originSize = QSize(100, 100);
    pri->curSize[0] = QSizeF(100, 100);
    pri->curSize[1] = QSizeF(100, 100);

    // Uncheck keep ratio so base becomes EFreeSetting.
    pri->_keepRaidoCheckBox->setChecked(false);
    pri->_widthEditor->setText("200");
    pri->_heightEditor->setText("200");

    bool result = pri->autoKeepSize(CExportImageDialog::CExportImageDialog_private::EKeepBaseW);
    // With free setting (no aspect lock), should succeed without alert.
    EXPECT_TRUE(result);

    delete content;
    delete dlg;
}

TEST(CExportImageDialogPrivateAutoKeepSize, KeepWidthAdjustsHeight)
{
    auto *dlg = makeDialog2();
    auto *pri = dlg->_pPrivate;

    QWidget *content = new QWidget();
    QFormLayout *layout = new QFormLayout(content);
    pri->initSizeSettingLayoutUi(layout, content);
    pri->originSize = QSize(100, 100);
    pri->curSize[0] = QSizeF(100, 100);
    pri->curSize[1] = QSizeF(100, 100);

    pri->_keepRaidoCheckBox->setChecked(true);
    pri->settingModel = CExportImageDialog::CExportImageDialog_private::EPixelModel;
    pri->_widthEditor->setText("50");
    pri->_heightEditor->setText("100");

    pri->autoKeepSize(CExportImageDialog::CExportImageDialog_private::EKeepBaseW);

    // With aspect ratio locked and base=W, height should be adjusted to 50.
    EXPECT_EQ(pri->curShowIntSize.width(), 50);

    delete content;
    delete dlg;
}
