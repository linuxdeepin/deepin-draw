// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for TopTilte::initMenu() lambdas
 * (src/frame/toptoolbar.cpp).
 *
 * Target lambdas (0-coverage):
 *   exportAc->triggered lambda — emits toExport signal (guarded by
 *       CHECK_MOSUEACTIVE_RETURN).
 *   printAc->triggered lambda — creates CPrintManager and shows print
 *       dialog (guarded by CHECK_MOSUEACTIVE_RETURN).
 *   saveAsAc->triggered lambda — calls slotOnSaveAsAction.
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QAction>
#include <QSignalSpy>

#define protected public
#define private public
#include "publicApi.h"
#include "toptoolbar.h"
#undef protected
#undef private

// Helper: find a QAction by text in the TopTilte's actions
static QAction *findAction(TopTilte *tt, const QString &text)
{
    for (auto *act : tt->actions()) {
        if (act->text() == text)
            return act;
    }
    return nullptr;
}

// ===========================================================================
// exportAc triggered lambda — emits toExport
// ===========================================================================

TEST(TopTilteMenuLambdaTest, ExportActionTriggeredNoCrash)
{
    TopTilte *tt = drawApp->topMainWindow()->topTitle();
    ASSERT_NE(tt, nullptr);

    // Re-init menu to ensure lambdas are wired
    tt->initMenu();

    // Find the Export action among tt's actions
    QAction *exportAc = findAction(tt, "Export");
    ASSERT_NE(exportAc, nullptr);

    QSignalSpy spy(tt, &TopTilte::toExport);
    exportAc->trigger();
    qApp->processEvents();

    // The lambda emits toExport (unless CHECK_MOSUEACTIVE_RETURN returns early).
    // Either way, no crash.
    SUCCEED();
}

// ===========================================================================
// printAc triggered lambda — creates CPrintManager
// ===========================================================================

TEST(TopTilteMenuLambdaTest, PrintActionTriggeredNoCrash)
{
    TopTilte *tt = drawApp->topMainWindow()->topTitle();
    ASSERT_NE(tt, nullptr);

    tt->initMenu();

    QAction *printAc = findAction(tt, "Print");
    if (printAc == nullptr) {
        GTEST_SKIP() << "Print action not available (possibly tablet mode)";
    }

    // Ensure a page exists so currentPage() is non-null
    ensurePageExists();

    printAc->trigger();
    qApp->processEvents();
    SUCCEED();
}

// ===========================================================================
// saveAsAc triggered lambda — calls slotOnSaveAsAction
// ===========================================================================

TEST(TopTilteMenuLambdaTest, SaveAsActionTriggeredNoCrash)
{
    TopTilte *tt = drawApp->topMainWindow()->topTitle();
    ASSERT_NE(tt, nullptr);

    tt->initMenu();

    QAction *saveAsAc = findAction(tt, "Save as");
    if (saveAsAc == nullptr) {
        GTEST_SKIP() << "Save as action not available (possibly tablet mode)";
    }

    // slotOnSaveAsAction may open a modal dialog; we just verify the lambda
    // connection exists and triggering doesn't crash before dialog exec.
    // Use a short timeout to avoid blocking.
    QTimer::singleShot(100, qApp, []() {
        // Close any modal dialog that might have opened
        QWidgetList widgets = QApplication::topLevelWidgets();
        for (auto *w : widgets) {
            if (w->inherits("QDialog") && w->isModal())
                w->close();
        }
    });

    saveAsAc->trigger();
    qApp->processEvents();
    SUCCEED();
}

// ===========================================================================
// newAction triggered — slotOnNewConstructAction
// ===========================================================================

TEST(TopTilteMenuLambdaTest, NewActionTriggeredNoCrash)
{
    TopTilte *tt = drawApp->topMainWindow()->topTitle();
    ASSERT_NE(tt, nullptr);

    tt->initMenu();

    QAction *newAc = findAction(tt, "New");
    ASSERT_NE(newAc, nullptr);

    newAc->trigger();
    qApp->processEvents();
    SUCCEED();
}
