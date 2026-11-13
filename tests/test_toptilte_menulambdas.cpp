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
#include <QApplication>
#include <QKeySequence>
#include <QSignalSpy>

#define protected public
#define private public
#include "publicApi.h"
#include "toptoolbar.h"
#undef protected
#undef private

#include "test_helpers.h"

namespace {

// The actions created by initMenu() carry no objectName and their text is
// translated, so match them on the stable non-translated shortcut instead.
const QKeySequence kNewShortcut(Qt::CTRL | Qt::Key_N);
const QKeySequence kSaveAsShortcut(Qt::CTRL | Qt::SHIFT | Qt::Key_S);
const QKeySequence kExportShortcut(Qt::CTRL | Qt::Key_E);
const QKeySequence kPrintShortcut(Qt::CTRL | Qt::Key_P);

QAction *findActionByShortcut(TopTilte *tt, const QKeySequence &shortcut)
{
    for (auto *act : tt->actions()) {
        if (act->shortcut() == shortcut)
            return act;
    }
    return nullptr;
}

// Restores the DrawBoard page state after every test: "New" adds a page and
// the export/print lambdas can touch the current page.
class TopTilteMenuLambdaTest : public PageStateGuard
{
protected:
    TopTilte *topTitle()
    {
        // SetUp() -> ensurePageExists() guarantees a current page; this also
        // keeps the saveAs/print lambdas on a meaningful code path.
        TopTilte *tt = drawApp->topMainWindow()->topTitle();
        EXPECT_NE(tt, nullptr);
        // The harness builds the menu at startup; only rebuild it when this
        // window has no actions yet. Re-running initMenu() on every test would
        // leave several actions per shortcut parented to the toolbar.
        if (tt != nullptr && findActionByShortcut(tt, kNewShortcut) == nullptr) {
            tt->initMenu();
        }
        return tt;
    }

    // A brand-new page has an empty PageContext, which keeps Page::saveAs()
    // on its early-return guard instead of opening a modal file dialog.
    Page *addFreshPage()
    {
        Page *page = drawApp->drawBoard()->addPage();
        EXPECT_NE(page, nullptr);
        return page;
    }
};

} // namespace

// ===========================================================================
// exportAc triggered lambda — emits toExport
// ===========================================================================

TEST_F(TopTilteMenuLambdaTest, ExportActionTriggeredNoCrash)
{
    TopTilte *tt = topTitle();
    ASSERT_NE(tt, nullptr);

    QAction *exportAc = findActionByShortcut(tt, kExportShortcut);
    ASSERT_NE(exportAc, nullptr);

    QSignalSpy spy(tt, &TopTilte::toExport);
    ASSERT_TRUE(spy.isValid());

    // Triggering the action emits toExport; MainWindow's handler opens the
    // (modal) export dialog, which the test harness auto-closes.
    exportAc->trigger();
    qApp->processEvents();

    EXPECT_EQ(spy.count(), 1);
    EXPECT_EQ(QApplication::activeModalWidget(), nullptr);
}

// ===========================================================================
// printAc triggered lambda — creates CPrintManager
// ===========================================================================

TEST_F(TopTilteMenuLambdaTest, PrintActionTriggeredNoCrash)
{
    TopTilte *tt = topTitle();
    ASSERT_NE(tt, nullptr);

    QAction *printAc = findActionByShortcut(tt, kPrintShortcut);
    if (printAc == nullptr) {
        GTEST_SKIP() << "Print action not available (possibly tablet mode)";
    }

    // The lambda only builds a CPrintManager when a page with a context
    // exists; the fixture's page provides that.
    Page *page = drawApp->drawBoard()->currentPage();
    ASSERT_NE(page, nullptr);
    ASSERT_NE(page->context(), nullptr);

    printAc->trigger();
    qApp->processEvents();

    // The preview dialog is modal and auto-closed by the harness; it must not
    // leak into the next test.
    EXPECT_EQ(QApplication::activeModalWidget(), nullptr);
}

// ===========================================================================
// saveAsAc triggered lambda — calls slotOnSaveAsAction
// ===========================================================================

TEST_F(TopTilteMenuLambdaTest, SaveAsActionTriggeredNoCrash)
{
    TopTilte *tt = topTitle();
    ASSERT_NE(tt, nullptr);

    QAction *saveAsAc = findActionByShortcut(tt, kSaveAsShortcut);
    if (saveAsAc == nullptr) {
        GTEST_SKIP() << "Save as action not available (possibly tablet mode)";
    }

    // A page must be current for slotOnSaveAsAction to reach saveAs(). Use a
    // fresh page: its empty context sends saveAs() to its isEmpty() guard
    // instead of opening a modal file dialog.
    Page *page = addFreshPage();
    ASSERT_NE(page, nullptr);
    ASSERT_NE(page->context(), nullptr);
    ASSERT_TRUE(page->context()->isEmpty());

    saveAsAc->trigger();
    qApp->processEvents();

    // The board still has the same current page and no dialog leaked.
    EXPECT_EQ(drawApp->drawBoard()->currentPage(), page);
    EXPECT_EQ(QApplication::activeModalWidget(), nullptr);
}

// ===========================================================================
// newAction triggered — slotOnNewConstructAction
// ===========================================================================

TEST_F(TopTilteMenuLambdaTest, NewActionTriggeredNoCrash)
{
    TopTilte *tt = topTitle();
    ASSERT_NE(tt, nullptr);

    QAction *newAc = findActionByShortcut(tt, kNewShortcut);
    ASSERT_NE(newAc, nullptr);

    QSignalSpy newSpy(tt, &TopTilte::creatOnePage);
    ASSERT_TRUE(newSpy.isValid());
    QSignalSpy addedSpy(drawApp->drawBoard(), &DrawBoard::pageAdded);
    ASSERT_TRUE(addedSpy.isValid());

    const int countBefore = drawApp->drawBoard()->count();

    newAc->trigger();

    // The lambda emits creatOnePage exactly once. Its MainWindow handler adds
    // one page synchronously (direct connection), so check before pumping the
    // event loop: unrelated queued work can add pages during processEvents().
    EXPECT_EQ(newSpy.count(), 1);

    // Do not assert an exact page delta of 1: other suites legitimately
    // re-invoke MainWindow::initConnection(), and Qt does not deduplicate
    // lambda connections, so creatOnePage can have more than one addPage()
    // handler. The invariant that must hold is one pageAdded per page, with
    // at least the one this action caused.
    EXPECT_GE(addedSpy.count(), 1);
    EXPECT_EQ(addedSpy.count(), drawApp->drawBoard()->count() - countBefore);

    qApp->processEvents();
}
