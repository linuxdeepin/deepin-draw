// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for MessageDlg events, SMessage, and GlobalShortcut
 * (src/widgets/dialog/dialog.cpp, src/utils/global.cpp).
 *
 * Complements test_dialog_msgdlg.cpp (which covers setMessage/updateMessage/
 * execMessage) by testing keyPressEvent, showEvent, SMessage construction,
 * and GlobalShortcut::instance.
 *
 * Issue V-7855: 7 test cases.
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QApplication>
#include <QKeyEvent>
#include <QTest>
#include <QTimer>
#include <QVariant>
#include <QWidget>

#define protected public
#define private public
#include "dialog.h"
#include "global.h"
#undef protected
#undef private

// =========================================================================
// MessageDlg::keyPressEvent
// =========================================================================

TEST(MessageDlgKeyPressEventTest, EscClosesDialog)
{
    MessageDlg dlg;
    dlg.show();
    ASSERT_TRUE(dlg.isVisible());

    QKeyEvent escEvent(QEvent::KeyPress, Qt::Key_Escape, Qt::NoModifier);
    QCoreApplication::sendEvent(&dlg, &escEvent);

    // After Esc (Cancel sequence), the dialog should be closed (hidden).
    EXPECT_FALSE(dlg.isVisible());
}

TEST(MessageDlgKeyPressEventTest, OtherKeyDoesNotClose)
{
    MessageDlg dlg;
    dlg.show();
    ASSERT_TRUE(dlg.isVisible());

    QKeyEvent keyEvent(QEvent::KeyPress, Qt::Key_A, Qt::NoModifier);
    QCoreApplication::sendEvent(&dlg, &keyEvent);

    // Non-Cancel key should not close the dialog.
    EXPECT_TRUE(dlg.isVisible());
    dlg.close();
}

// =========================================================================
// MessageDlg::showEvent
// =========================================================================

TEST(MessageDlgShowEventTest, CentersOnParentWindow)
{
    QWidget parentWidget;
    parentWidget.resize(800, 600);
    parentWidget.move(0, 0);
    parentWidget.show();
    ASSERT_TRUE(QTest::qWaitForWindowExposed(&parentWidget));

    MessageDlg dlg(SMessage("Center test"), &parentWidget);
    dlg.show();

    // showEvent uses queued connections to reposition; process events.
    QCoreApplication::processEvents(QEventLoop::AllEvents, 500);

    // The dialog should be visible and not crash during centering.
    EXPECT_TRUE(dlg.isVisible());
    dlg.close();
    parentWidget.close();
}

// =========================================================================
// SMessage
// =========================================================================

TEST(SMessageTest, DefaultConstruction)
{
    SMessage msg;
    EXPECT_TRUE(msg.message.isEmpty());
    EXPECT_EQ(msg.maxMsgPixelLenth, 1920);
    EXPECT_EQ(msg.messageType, EWarningMsg);
    EXPECT_EQ(msg.btns.size(), 1);
    EXPECT_EQ(msg.btnType.size(), 1);
}

TEST(SMessageTest, CustomConstruction)
{
    QStringList btns;
    btns << "Yes" << "No";
    QList<EButtonType> btnTypes;
    btnTypes << ESuggestedMsgBtn << ENormalMsgBtn;

    SMessage msg("Custom message", EQuestionMsg, btns, btnTypes);
    EXPECT_EQ(msg.message, QString("Custom message"));
    EXPECT_EQ(msg.messageType, EQuestionMsg);
    EXPECT_EQ(msg.btns.size(), 2);
    EXPECT_EQ(msg.btnType.size(), 2);
    EXPECT_EQ(msg.btns.at(0), QString("Yes"));
    EXPECT_EQ(msg.btnType.at(0), ESuggestedMsgBtn);
}

TEST(SMessageTest, QVariantRoundTrip)
{
    SMessage msg("Variant test", ENormalMsg);
    // SMessage is registered via Q_DECLARE_METATYPE; store and retrieve.
    QVariant var = QVariant::fromValue(msg);
    ASSERT_TRUE(var.isValid());
    ASSERT_TRUE(var.canConvert<SMessage>());

    SMessage recovered = var.value<SMessage>();
    EXPECT_EQ(recovered.message, QString("Variant test"));
    EXPECT_EQ(recovered.messageType, ENormalMsg);
    EXPECT_EQ(recovered.maxMsgPixelLenth, 1920);
}

// =========================================================================
// GlobalShortcut::instance
// =========================================================================

TEST(GlobalShortcutInstanceTest, SingletonReturnsSameInstance)
{
    GlobalShortcut *inst1 = GlobalShortcut::instance();
    ASSERT_NE(inst1, nullptr);
    GlobalShortcut *inst2 = GlobalShortcut::instance();
    EXPECT_EQ(inst1, inst2);
}
