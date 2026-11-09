// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for CCutDialog and DrawDialog (issue V-7935)
 *
 * CCutDialog target:
 *   - Constructor attributes (objectName / accessibleName / modal / default Discard / two buttons)
 *   - keyPressEvent Esc sets Discard and closes
 *   - buttonClicked(0/1) switches cut status and closes
 *   - exec() returns getCutStatus() rather than QDialog result code
 *   - showEvent does not crash (centering on parent window)
 *
 * DrawDialog target:
 *   - Constructor attributes (accessibleName / modal / three buttons)
 *   - keyPressEvent Esc closes the dialog
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QtConcurrent/QtConcurrent>
#include <QKeyEvent>
#include <QShowEvent>
#include <QTest>
#include <QAbstractButton>
#include <QTimer>

// application.h internally does #undef protected, so include it BEFORE
// the #define protected public block.
#include "application.h"

#define protected public
#define private public
#include "ccutdialog.h"
#include "drawdialog.h"
#undef protected
#undef private

// =========================================================================
// CCutDialog — constructor attributes
// =========================================================================

TEST(CCutDialogFuncTest, Constructor_ObjectNameIsCutDialog)
{
    CCutDialog dialog;
    EXPECT_EQ(dialog.objectName(), QStringLiteral("CutDialog"));
}

TEST(CCutDialogFuncTest, Constructor_AccessibleName)
{
    CCutDialog dialog;
    EXPECT_EQ(dialog.accessibleName(), QStringLiteral("Notice cut info dialog"));
}

TEST(CCutDialogFuncTest, Constructor_IsModal)
{
    CCutDialog dialog;
    EXPECT_TRUE(dialog.isModal());
}

TEST(CCutDialogFuncTest, Constructor_DefaultCutStatusIsDiscard)
{
    CCutDialog dialog;
    EXPECT_EQ(dialog.getCutStatus(), CCutDialog::Discard);
}

TEST(CCutDialogFuncTest, Constructor_HasTwoButtons)
{
    CCutDialog dialog;
    // Discard (index 0) and Save (index 1)
    EXPECT_NE(dialog.getButton(0), nullptr);
    EXPECT_NE(dialog.getButton(1), nullptr);
}

TEST(CCutDialogFuncTest, Constructor_SaveButtonObjectName)
{
    CCutDialog dialog;
    ASSERT_NE(dialog.getButton(1), nullptr);
    EXPECT_EQ(dialog.getButton(1)->objectName(), QStringLiteral("CutDialogSave"));
}

// =========================================================================
// CCutDialog — keyPressEvent Esc sets Discard and closes
// =========================================================================

TEST(CCutDialogFuncTest, KeyPressEvent_Esc_SetsDiscard)
{
    CCutDialog dialog;
    // First switch status away from Discard so we can observe the change.
    QMetaObject::invokeMethod(&dialog, "buttonClicked", Qt::DirectConnection,
                              Q_ARG(int, 1), Q_ARG(const QString &, QString()));
    ASSERT_EQ(dialog.getCutStatus(), CCutDialog::Save);

    // Pressing Esc should reset to Discard.
    QKeyEvent escEvent(QEvent::KeyPress, Qt::Key_Escape, Qt::NoModifier);
    dialog.keyPressEvent(&escEvent);

    EXPECT_EQ(dialog.getCutStatus(), CCutDialog::Discard);
}

TEST(CCutDialogFuncTest, KeyPressEvent_NonEscape_DoesNotChangeStatus)
{
    CCutDialog dialog;
    QMetaObject::invokeMethod(&dialog, "buttonClicked", Qt::DirectConnection,
                              Q_ARG(int, 1), Q_ARG(const QString &, QString()));
    ASSERT_EQ(dialog.getCutStatus(), CCutDialog::Save);

    QKeyEvent keyEvent(QEvent::KeyPress, Qt::Key_Return, Qt::NoModifier);
    dialog.keyPressEvent(&keyEvent);

    // Non-Escape key should not alter the cut status.
    EXPECT_EQ(dialog.getCutStatus(), CCutDialog::Save);
}

// =========================================================================
// CCutDialog — buttonClicked(0/1) switches cut status and closes
// =========================================================================

TEST(CCutDialogFuncTest, ButtonClicked_Index1_SetsSave)
{
    CCutDialog dialog;
    ASSERT_EQ(dialog.getCutStatus(), CCutDialog::Discard);

    QMetaObject::invokeMethod(&dialog, "buttonClicked", Qt::DirectConnection,
                              Q_ARG(int, 1), Q_ARG(const QString &, QString()));

    EXPECT_EQ(dialog.getCutStatus(), CCutDialog::Save);
}

TEST(CCutDialogFuncTest, ButtonClicked_Index0_SetsDiscard)
{
    CCutDialog dialog;
    // First set to Save, then use button 0 to set back to Discard.
    QMetaObject::invokeMethod(&dialog, "buttonClicked", Qt::DirectConnection,
                              Q_ARG(int, 1), Q_ARG(const QString &, QString()));
    ASSERT_EQ(dialog.getCutStatus(), CCutDialog::Save);

    QMetaObject::invokeMethod(&dialog, "buttonClicked", Qt::DirectConnection,
                              Q_ARG(int, 0), Q_ARG(const QString &, QString()));

    EXPECT_EQ(dialog.getCutStatus(), CCutDialog::Discard);
}

TEST(CCutDialogFuncTest, ButtonClicked_UnknownIndex_DoesNotChangeStatus)
{
    CCutDialog dialog;
    QMetaObject::invokeMethod(&dialog, "buttonClicked", Qt::DirectConnection,
                              Q_ARG(int, 1), Q_ARG(const QString &, QString()));
    ASSERT_EQ(dialog.getCutStatus(), CCutDialog::Save);

    // An out-of-range index (2) should not match either branch.
    QMetaObject::invokeMethod(&dialog, "buttonClicked", Qt::DirectConnection,
                              Q_ARG(int, 2), Q_ARG(const QString &, QString()));

    EXPECT_EQ(dialog.getCutStatus(), CCutDialog::Save);
}

// =========================================================================
// CCutDialog — exec() returns getCutStatus() rather than QDialog result code
// =========================================================================

TEST(CCutDialogFuncTest, Exec_ReturnsCutStatus_NotQDialogResult)
{
    CCutDialog dialog;
    // Default status is Discard (value 2), which differs from any standard
    // QDialog::DialogCode (Accepted=1, Rejected=0).  This lets us distinguish
    // the custom return value from a plain QDialog result code.
    ASSERT_EQ(dialog.getCutStatus(), CCutDialog::Discard);

    int execResult = -999;
    // Invoke exec() in the main thread so the modal event loop works
    // correctly; the global test harness auto-closes modal dialogs by
    // calling done(-1), and CCutDialog::exec() ignores that result and
    // returns getCutStatus().
    QTimer::singleShot(0, qApp, [&dialog, &execResult]() {
        execResult = dialog.exec();
    });

    // Safety net: if the auto-close doesn't fire, force-close after 5s.
    QTimer::singleShot(5000, qApp, [&dialog]() {
        if (qApp->activeModalWidget() == &dialog) {
            dialog.done(-1);
        }
    });

    // Process events until exec() returns (or timeout).
    (void)QTest::qWaitFor([&execResult]() {
        return execResult != -999;
    }, 15000);

    // Should be Discard (2), not the QDialog done code (-1 or 0).
    EXPECT_EQ(execResult, static_cast<int>(CCutDialog::Discard));
}

// =========================================================================
// CCutDialog — showEvent does not crash (centering on parent window)
// =========================================================================

TEST(CCutDialogFuncTest, ShowEvent_WithParent_DoesNotCrash)
{
    CCutDialog dialog(drawApp->topMainWindowWidget());
    QShowEvent showEvent;
    // Calling showEvent directly posts the centering lambdas to the event
    // queue; process events so they run without crashing.
    dialog.showEvent(&showEvent);
    QTest::qWait(200);

    SUCCEED();
}

// =========================================================================
// DrawDialog — constructor attributes
// =========================================================================

TEST(DrawDialogFuncTest, Constructor_AccessibleName)
{
    DrawDialog dialog;
    EXPECT_EQ(dialog.accessibleName(), QStringLiteral("Notice save dialog"));
}

TEST(DrawDialogFuncTest, Constructor_IsModal)
{
    DrawDialog dialog;
    EXPECT_TRUE(dialog.isModal());
}

TEST(DrawDialogFuncTest, Constructor_HasThreeButtons)
{
    DrawDialog dialog;
    // Cancel (0), Discard (1), Save (2)
    EXPECT_NE(dialog.getButton(0), nullptr);
    EXPECT_NE(dialog.getButton(1), nullptr);
    EXPECT_NE(dialog.getButton(2), nullptr);
}

// =========================================================================
// DrawDialog — keyPressEvent Esc closes the dialog
// =========================================================================

TEST(DrawDialogFuncTest, KeyPressEvent_Esc_DoesNotCrash)
{
    DrawDialog dialog;
    QKeyEvent escEvent(QEvent::KeyPress, Qt::Key_Escape, Qt::NoModifier);
    // The handler calls close(); on a non-visible dialog this is harmless.
    dialog.keyPressEvent(&escEvent);
    SUCCEED();
}

TEST(DrawDialogFuncTest, KeyPressEvent_NonEscape_DoesNotCrash)
{
    DrawDialog dialog;
    QKeyEvent keyEvent(QEvent::KeyPress, Qt::Key_Tab, Qt::NoModifier);
    dialog.keyPressEvent(&keyEvent);
    SUCCEED();
}

// =========================================================================
// DrawDialog — showEvent does not crash
// =========================================================================

TEST(DrawDialogFuncTest, ShowEvent_WithParent_DoesNotCrash)
{
    DrawDialog dialog(drawApp->topMainWindowWidget());
    QShowEvent showEvent;
    dialog.showEvent(&showEvent);
    QTest::qWait(200);
    SUCCEED();
}
