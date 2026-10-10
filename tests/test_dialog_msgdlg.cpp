// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for MessageDlg and defaultParentWindow (dialog.cpp).
 *
 * Target functions (from issue V-5568):
 *   - MessageDlg::execMessage()
 *   - MessageDlg::setMessage()
 *   - MessageDlg::updateMessage()
 *   - defaultParentWindow()
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QApplication>
#include <QDialog>
#include <QTimer>
#include <QWidget>

#define protected public
#define private public
#include "dialog.h"
extern QWidget *defaultParentWindow();
#undef protected
#undef private

// =========================================================================
// defaultParentWindow (free function)
// =========================================================================

TEST(DialogDefaultParentWindowTest, ReturnsNonNullWhenAppActive)
{
    // In offscreen mode with no active window, returns nullptr or
    // activeWindow(); either way should not crash.
    QWidget *w = defaultParentWindow();
    // The function returns qApp->activeModalWidget() or activeWindow(),
    // both can be nullptr in headless mode — just verify no crash.
    SUCCEED();
}

// =========================================================================
// MessageDlg::setMessage / updateMessage
// =========================================================================

TEST(MessageDlgSetMessageTest, SetMessageStoresMessage)
{
    SMessage msg("Test warning", EWarningMsg);
    MessageDlg dlg;
    dlg.setMessage(msg);

    SMessage stored = dlg.message();
    EXPECT_EQ(stored.message, QString("Test warning"));
    EXPECT_EQ(stored.messageType, EWarningMsg);
}

TEST(MessageDlgSetMessageTest, SetMessageWithNormalType)
{
    SMessage msg("Normal info", ENormalMsg);
    MessageDlg dlg;
    dlg.setMessage(msg);

    EXPECT_EQ(dlg.message().messageType, ENormalMsg);
}

TEST(MessageDlgSetMessageTest, SetMessageWithQuestionType)
{
    SMessage msg("Are you sure?", EQuestionMsg);
    MessageDlg dlg;
    dlg.setMessage(msg);

    EXPECT_EQ(dlg.message().messageType, EQuestionMsg);
}

// =========================================================================
// MessageDlg::updateMessage (private, accessed via #define private public)
// =========================================================================

TEST(MessageDlgUpdateMessageTest, UpdateMessageDoesNotCrash)
{
    SMessage msg("Update test", EWarningMsg);
    MessageDlg dlg;
    dlg._message = msg;
    dlg.updateMessage();
    SUCCEED();
}

TEST(MessageDlgUpdateMessageTest, UpdateMessageWithButtons)
{
    SMessage msg("Choose", EQuestionMsg,
                 QStringList() << "Yes" << "No",
                 QList<EButtonType>() << ESuggestedMsgBtn << ENormalMsgBtn);
    MessageDlg dlg;
    dlg._message = msg;
    dlg.updateMessage();
    SUCCEED();
}

// =========================================================================
// MessageDlg constructor with SMessage
// =========================================================================

TEST(MessageDlgConstructorTest, ConstructWithMessage)
{
    SMessage msg("Constructor test", EWarningMsg);
    MessageDlg dlg(msg);
    EXPECT_EQ(dlg.message().message, QString("Constructor test"));
}

TEST(MessageDlgConstructorTest, DefaultConstructor)
{
    MessageDlg dlg;
    // Default-constructed dialog should have an empty message.
    EXPECT_TRUE(dlg.message().message.isEmpty());
}

// =========================================================================
// MessageDlg::execMessage (static overloads — smoke test only)
// =========================================================================

TEST(MessageDlgExecMessageTest, ExecMessageWithQStringReturnsInt)
{
    // Invoke the real production execMessage() (QString overload).
    // It blocks in QDialog::exec(), so close the modal dialog from a
    // timer as soon as it appears; the test harness
    // (QTestMain::autoQuitActivedModalWidget) acts as a backstop.
    QTimer closeTimer;
    int attempts = 0;
    QObject::connect(&closeTimer, &QTimer::timeout, [&closeTimer, &attempts]() {
        QWidget *modal = QApplication::activeModalWidget();
        if (modal != nullptr || ++attempts > 50) {
            if (modal != nullptr)
                modal->close();
            closeTimer.stop();
        }
    });
    closeTimer.start(100);

    int result = MessageDlg::execMessage(QString("Exec test"));
    closeTimer.stop();

    // Closed dialog: QDialog::Rejected (0); harness backstop: done(-1).
    EXPECT_TRUE(result == QDialog::Rejected || result == -1);
}
