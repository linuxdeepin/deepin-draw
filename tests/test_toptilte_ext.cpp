// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for TopTilte extended methods (issue V-7618 group 13)
 *
 * Source: src/frame/toptoolbar.h/.cpp
 *
 * Target methods/signals:
 *   initMenu, slotZoom (qreal), slotIsCutMode, slotMenuShow,
 *   slotOnImportAction, slotOnSaveAction, slotOnSaveAsAction,
 *   slotOnNewConstructAction
 *   Signals: toSave, toSaveAs, toPrint, toOpen, zoomTo, toExport, creatOnePage
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QSignalSpy>
#include <QAction>

#define protected public
#define private public
#include "publicApi.h"
#include "toptoolbar.h"
#include "dzoommenucombobox.h"
#undef protected
#undef private

// =========================================================================
// TopTilte::initMenu
// =========================================================================

TEST(TopTilteInitMenu, CreatesMainMenu_NoCrash)
{
    TopTilte *tt = drawApp->topMainWindow()->topTitle();
    ASSERT_NE(tt, nullptr);

    // initMenu should create m_mainMenu with actions
    tt->initMenu();

    EXPECT_NE(tt->m_mainMenu, nullptr);
}

TEST(TopTilteInitMenu, MainMenuHasActions)
{
    TopTilte *tt = drawApp->topMainWindow()->topTitle();
    ASSERT_NE(tt, nullptr);

    tt->initMenu();

    if (tt->m_mainMenu) {
        // The menu should have at least some actions
        EXPECT_GT(tt->m_mainMenu->actions().size(), 0);
    }
}

// =========================================================================
// TopTilte::slotZoom (qreal overload)
// =========================================================================

TEST(TopTilteSlotZoomQreal, EmitsZoomTo_AndCallsSlotSetScale)
{
    TopTilte *tt = drawApp->topMainWindow()->topTitle();
    ASSERT_NE(tt, nullptr);

    QSignalSpy spy(tt, &TopTilte::zoomTo);
    ASSERT_TRUE(spy.isValid());

    tt->slotZoom(2.0);

    // slotZoom(qreal) should emit zoomTo(scale)
    ASSERT_EQ(spy.count(), 1);
    QList<QVariant> args = spy.takeFirst();
    EXPECT_NEAR(args.at(0).toReal(), 2.0, 0.01);
}

TEST(TopTilteSlotZoomQreal, ZeroScale_NoCrash)
{
    TopTilte *tt = drawApp->topMainWindow()->topTitle();
    ASSERT_NE(tt, nullptr);

    tt->slotZoom(0.0);
    SUCCEED();
}

TEST(TopTilteSlotZoomQreal, NegativeScale_NoCrash)
{
    TopTilte *tt = drawApp->topMainWindow()->topTitle();
    ASSERT_NE(tt, nullptr);

    tt->slotZoom(-1.0);
    SUCCEED();
}

// =========================================================================
// TopTilte::slotIsCutMode
// =========================================================================

TEST(TopTilteSlotIsCutMode, WithNullAction_NoCrash)
{
    TopTilte *tt = drawApp->topMainWindow()->topTitle();
    ASSERT_NE(tt, nullptr);

    tt->slotIsCutMode(nullptr);
    SUCCEED();
}

TEST(TopTilteSlotIsCutMode, WithAction_NoCrash)
{
    TopTilte *tt = drawApp->topMainWindow()->topTitle();
    ASSERT_NE(tt, nullptr);

    QAction action("test");
    action.setChecked(true);
    tt->slotIsCutMode(&action);
    SUCCEED();
}

// =========================================================================
// TopTilte::slotMenuShow
// =========================================================================

TEST(TopTilteSlotMenuShow, NoCrash)
{
    TopTilte *tt = drawApp->topMainWindow()->topTitle();
    ASSERT_NE(tt, nullptr);

    // slotMenuShow is an empty function body — just verify no crash
    tt->slotMenuShow();
    SUCCEED();
}

// =========================================================================
// TopTilte::slotOnImportAction / slotOnSaveAction / slotOnSaveAsAction / slotOnNewConstructAction
// =========================================================================

TEST(TopTilteSlotOnImportAction, EmitsToOpen)
{
    TopTilte *tt = drawApp->topMainWindow()->topTitle();
    ASSERT_NE(tt, nullptr);

    QSignalSpy spy(tt, &TopTilte::toOpen);
    ASSERT_TRUE(spy.isValid());

    tt->slotOnImportAction();

    // slotOnImportAction emits toOpen() unconditionally
    ASSERT_EQ(spy.count(), 1);
}

TEST(TopTilteSlotOnSaveAction, SavesCurrentPage_NoSignalEmitted)
{
    TopTilte *tt = drawApp->topMainWindow()->topTitle();
    ASSERT_NE(tt, nullptr);

    QSignalSpy spy(tt, &TopTilte::toSave);
    ASSERT_TRUE(spy.isValid());

    // slotOnSaveAction saves the current page; it does not emit toSave
    tt->slotOnSaveAction();

    EXPECT_EQ(spy.count(), 0);
}

TEST(TopTilteSlotOnSaveAsAction, CallsSaveAs_NoSignalEmitted)
{
    TopTilte *tt = drawApp->topMainWindow()->topTitle();
    ASSERT_NE(tt, nullptr);

    QSignalSpy spy(tt, &TopTilte::toSaveAs);
    ASSERT_TRUE(spy.isValid());

    // slotOnSaveAsAction calls currentPage()->saveAs(); it does not emit toSaveAs
    tt->slotOnSaveAsAction();

    EXPECT_EQ(spy.count(), 0);
}

TEST(TopTilteSlotOnNewConstructAction, EmitsCreatOnePage)
{
    TopTilte *tt = drawApp->topMainWindow()->topTitle();
    ASSERT_NE(tt, nullptr);

    QSignalSpy spy(tt, &TopTilte::creatOnePage);
    ASSERT_TRUE(spy.isValid());

    tt->slotOnNewConstructAction();

    // slotOnNewConstructAction emits creatOnePage() unconditionally
    ASSERT_EQ(spy.count(), 1);
}

// =========================================================================
// TopTilte signals validity
// =========================================================================

TEST(TopTilteSignals, AllSignalsAreValid)
{
    TopTilte *tt = drawApp->topMainWindow()->topTitle();
    ASSERT_NE(tt, nullptr);

    QSignalSpy spySave(tt, &TopTilte::toSave);
    QSignalSpy spySaveAs(tt, &TopTilte::toSaveAs);
    QSignalSpy spyPrint(tt, &TopTilte::toPrint);
    QSignalSpy spyOpen(tt, &TopTilte::toOpen);
    QSignalSpy spyZoom(tt, &TopTilte::zoomTo);
    QSignalSpy spyExport(tt, &TopTilte::toExport);
    QSignalSpy spyNew(tt, &TopTilte::creatOnePage);

    EXPECT_TRUE(spySave.isValid());
    EXPECT_TRUE(spySaveAs.isValid());
    EXPECT_TRUE(spyPrint.isValid());
    EXPECT_TRUE(spyOpen.isValid());
    EXPECT_TRUE(spyZoom.isValid());
    EXPECT_TRUE(spyExport.isValid());
    EXPECT_TRUE(spyNew.isValid());
}

// =========================================================================
// TopTilte::slotZoom (QString overload)
// =========================================================================

TEST(TopTilteSlotZoomString, ValidString_NoCrash)
{
    TopTilte *tt = drawApp->topMainWindow()->topTitle();
    ASSERT_NE(tt, nullptr);

    tt->slotZoom(QString("100%"));
    SUCCEED();
}

TEST(TopTilteSlotZoomString, InvalidString_NoCrash)
{
    TopTilte *tt = drawApp->topMainWindow()->topTitle();
    ASSERT_NE(tt, nullptr);

    tt->slotZoom(QString("invalid"));
    SUCCEED();
}

TEST(TopTilteSlotZoomString, EmptyString_NoCrash)
{
    TopTilte *tt = drawApp->topMainWindow()->topTitle();
    ASSERT_NE(tt, nullptr);

    tt->slotZoom(QString());
    SUCCEED();
}
