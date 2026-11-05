// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for interfaces not covered by test_dialog_msgdlg.cpp:
 * MessageDlg::keyPressEvent / showEvent, SMessage defaults and QVariant
 * round-trip, and the GlobalShortcut singleton identity.
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QApplication>
#include <QObject>
#include <QStringList>
#include <QTest>
#include <QVariant>
#include <QWidget>

#include "dialog.h"
#include "global.h"

// --- MessageDlg::keyPressEvent ---

TEST(MessageDlgKeyPressTest, EscapeClosesDialog)
{
    MessageDlg dlg;
    dlg.show();
    ASSERT_TRUE(dlg.isVisible());

    QTest::keyClick(&dlg, Qt::Key_Escape);

    EXPECT_FALSE(dlg.isVisible());
}

TEST(MessageDlgKeyPressTest, OtherKeyDoesNotCloseDialog)
{
    MessageDlg dlg;
    dlg.show();
    ASSERT_TRUE(dlg.isVisible());

    QTest::keyClick(&dlg, Qt::Key_A);

    EXPECT_TRUE(dlg.isVisible());
    dlg.close();
}

// --- MessageDlg::showEvent ---

TEST(MessageDlgShowEventTest, DialogCentersOnParentWindow)
{
    QWidget parent;
    parent.setGeometry(100, 100, 800, 600);
    parent.show();
    MessageDlg dlg(&parent);

    dlg.show();

    // showEvent recenters via a doubly-queued invocation; pump the loop
    // until the position settles.
    QPoint expected;
    for (int i = 0; i < 50; ++i) {
        QApplication::processEvents();
        QTest::qWait(10);
        const QRect parentGem = parent.geometry();
        expected = parentGem.topLeft()
                   + QPoint((parentGem.width() - dlg.width()) / 2,
                            (parentGem.height() - dlg.height()) / 2);
        if (dlg.pos() == expected) {
            break;
        }
    }

    EXPECT_EQ(dlg.pos(), expected);
    dlg.close();
}

// --- SMessage ---

TEST(SMessageTest, DefaultConstructorFillsDefaults)
{
    SMessage msg;

    EXPECT_TRUE(msg.message.isEmpty());
    EXPECT_EQ(msg.maxMsgPixelLenth, 1920);
    EXPECT_EQ(msg.messageType, EWarningMsg);
    EXPECT_EQ(msg.btns, QStringList() << QObject::tr("OK"));
    EXPECT_EQ(msg.btnType, QList<EButtonType>() << ENormalMsgBtn);
}

TEST(SMessageTest, CustomConstructorStoresAllFields)
{
    QStringList btns;
    btns << "Yes" << "No";
    QList<EButtonType> types;
    types << ESuggestedMsgBtn << ENormalMsgBtn;

    SMessage msg("Save changes?", EQuestionMsg, btns, types);

    EXPECT_EQ(msg.message, QString("Save changes?"));
    EXPECT_EQ(msg.messageType, EQuestionMsg);
    EXPECT_EQ(msg.btns, btns);
    EXPECT_EQ(msg.btnType, types);
}

TEST(SMessageTest, QVariantRoundTripKeepsFields)
{
    SMessage original("round trip", ENormalMsg);

    QVariant var = QVariant::fromValue(original);
    ASSERT_TRUE(var.canConvert<SMessage>());

    SMessage restored = var.value<SMessage>();
    EXPECT_EQ(restored.message, original.message);
    EXPECT_EQ(restored.messageType, original.messageType);
    EXPECT_EQ(restored.btns, original.btns);
}

// --- GlobalShortcut ---

TEST(GlobalShortcutTest, InstanceIsSingletonAndNonNull)
{
    GlobalShortcut *first = GlobalShortcut::instance();
    GlobalShortcut *second = GlobalShortcut::instance();

    ASSERT_NE(first, nullptr);
    EXPECT_EQ(first, second);
}
