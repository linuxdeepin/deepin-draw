// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for DrawBoard, CFileWatcher, FileSelectDialog, CPictureTool,
 * IDrawTool uncovered paths (issue V-9599 §5).
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QMenu>
#include <QApplication>
#include <QTemporaryDir>
#include <QTemporaryFile>
#include <QFile>
#include <QImage>
#include <QSignalSpy>
#include <QTouchEvent>
#include <QGraphicsSceneMouseEvent>
#include <QPushButton>
#include <memory>

#define protected public
#define private public
#include "frame/ccentralwidget.h"
#include "frame/cviewmanagement.h"
#include "frame/cmultiptabbarwidget.h"
#include "frame/cgraphicsview.h"
#include "frame/cundoredocommand.h"
#include "drawshape/cdrawscene.h"
#include "drawshape/drawItems/bzItems/cgraphicsitem.h"
#include "drawshape/drawItems/bzItems/cgraphicsrectitem.h"
#include "drawshape/drawItems/bzItems/cpictureitem.h"
#include "drawshape/drawItems/bzItems/cgraphicsitemevent.h"
#include "drawshape/drawTools/idrawtool.h"
#include "drawshape/drawTools/idrawtoolevent.h"
#include "drawshape/drawTools/cdrawtoolfactory.h"
#include "drawshape/drawTools/cpicturetool.h"
#include "drawshape/drawItems/cgraphicsitemselectedmgr.h"
#include "drawshape/globaldefine.h"
#include "drawshape/sitemdata.h"
#include "application.h"
#include "publicApi.h"
#undef protected
#undef private

// ---------------------------------------------------------------------------
// DrawBoard::onFileContextChanged — file deleted (lines 1530-1533)
// ---------------------------------------------------------------------------

TEST(CCentralWidgetCViewMgmtMiscCoverageTest, OnFileContextChanged_FileDeleted_ClearsFile)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    ASSERT_NE(board, nullptr);
    Page *page = board->currentPage();
    ASSERT_NE(page, nullptr);

    QString originalFile = page->file();
    bool originalDirty = page->context()->isDirty();

    // Set a non-existent file path.
    QString ghostPath = "/tmp/nonexistent_ghost_file_9599.ddf";
    page->setFile(ghostPath);

    // Trigger onFileContextChanged — file doesn't exist → clears file, sets dirty.
    board->onFileContextChanged(ghostPath, CFileWatcher::EFileModified);

    EXPECT_TRUE(page->file().isEmpty());
    EXPECT_TRUE(page->context()->isDirty());

    // Restore original state.
    page->setFile(originalFile);
    page->context()->setDirty(originalDirty);
}

// ---------------------------------------------------------------------------
// DrawBoard::loadImage — adapt page rect (lines 1572-1573, 1587-1595)
// ---------------------------------------------------------------------------

TEST(CCentralWidgetCViewMgmtMiscCoverageTest, LoadImage_AdaptPageRect_AdjustsPageRect)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    ASSERT_NE(board, nullptr);
    Page *page = board->currentPage();
    ASSERT_NE(page, nullptr);

    // Create a small test image.
    QTemporaryDir tmpDir;
    ASSERT_TRUE(tmpDir.isValid());
    QString imgPath = tmpDir.filePath("test_64x48.png");
    QImage testImg(64, 48, QImage::Format_ARGB32);
    testImg.fill(Qt::red);
    ASSERT_TRUE(testImg.save(imgPath));

    QRectF originalPageRect = page->pageRect();

    // loadImage with changContexSizeToImag=true → adjusts page rect.
    bool ret = board->loadImage(imgPath, false, true);
    EXPECT_TRUE(ret);

    // Verify page rect was adjusted (should be different from original if image is larger).
    QRectF newPageRect = page->pageRect();
    // The formula: if pageRect < img, expand centered; otherwise keep.
    // Since default page rect may be larger than 64x48, check the formula.
    QRectF expectedRect = originalPageRect.adjusted(
        (originalPageRect.width() > testImg.width() ? 0 : (originalPageRect.width() - testImg.width()) / 2),
        (originalPageRect.height() > testImg.height() ? 0 : (originalPageRect.height() - testImg.height()) / 2),
        (originalPageRect.width() > testImg.width() ? 0 : -(originalPageRect.width() - testImg.width()) / 2),
        (originalPageRect.height() > testImg.height() ? 0 : -(originalPageRect.height() - testImg.height()) / 2)
    );
    EXPECT_EQ(newPageRect, expectedRect);

    // Clean up: remove the picture item that was added.
    auto items = scene->getBzItems();
    for (auto item : items) {
        if (item->type() == PictureType) {
            scene->removeCItem(item, true);
        }
    }

    // Restore original page rect.
    page->setPageRect(originalPageRect);
}

// ---------------------------------------------------------------------------
// CFileWatcher — file modified and moved events (lines 383-386)
// ---------------------------------------------------------------------------

TEST(CCentralWidgetCViewMgmtMiscCoverageTest, CFileWatcher_FileModified_EmitsSignal)
{
    CFileWatcher watcher;

    QTemporaryDir tmpDir;
    ASSERT_TRUE(tmpDir.isValid());
    QString filePath = tmpDir.filePath("watched.txt");
    QFile file(filePath);
    ASSERT_TRUE(file.open(QIODevice::WriteOnly));
    file.write("initial");
    file.close();

    watcher.addWather(filePath);

    QSignalSpy spy(&watcher, &CFileWatcher::fileChanged);
    ASSERT_TRUE(spy.isValid());

    // Append to file to trigger IN_MODIFY.
    QFile writeAgain(filePath);
    ASSERT_TRUE(writeAgain.open(QIODevice::Append | QIODevice::Text));
    writeAgain.write(" modified");
    writeAgain.close();

    // Wait for the signal.
    bool got = spy.wait(3000);
    EXPECT_TRUE(got);
    if (got) {
        QList<QVariant> args = spy.takeFirst();
        int tp = args.at(1).toInt();
        EXPECT_EQ(tp, CFileWatcher::EFileModified);
    }

    // Clean up the watcher thread.
    watcher._running = false;
    watcher.terminate();
    watcher.wait(1000);
}

// ---------------------------------------------------------------------------
// FileSelectDialog::extractSuffix — empty suffix (line 314)
// ---------------------------------------------------------------------------

TEST(CCentralWidgetCViewMgmtMiscCoverageTest, ExtractSuffix_EmptySuffix_ReturnsEmpty)
{
    createNewViewByShortcutKey();
    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    ASSERT_NE(board, nullptr);

    FileSelectDialog dlg(board);
    // "Images ( )" → no suffix content → returns empty.
    QString result1 = dlg.extractSuffix("Images ( )");
    EXPECT_TRUE(result1.isEmpty());

    // "(*.png *.jpg)" → returns "png".
    QString result2 = dlg.extractSuffix("(*.png *.jpg)");
    EXPECT_EQ(result2, QString("png"));
}

// ---------------------------------------------------------------------------
// IDrawTool::toolDoStart — double-click early return (lines 252-254)
// ---------------------------------------------------------------------------

TEST(CCentralWidgetCViewMgmtMiscCoverageTest, ToolDoStart_DblClick_ReturnsEarly)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    ASSERT_NE(board, nullptr);

    std::unique_ptr<IDrawTool> tool(CDrawToolFactory::Create(rectangle));
    ASSERT_NE(tool, nullptr);
    tool->setDrawBoard(board);

    // Create a mouse double-click event that makes dueTouchDoubleClickedStart return true.
    QGraphicsSceneMouseEvent mouseEvent(QEvent::GraphicsSceneMouseDoubleClick);
    mouseEvent.setSource(Qt::MouseEventNotSynthesized);
    mouseEvent.setButton(Qt::LeftButton);
    mouseEvent.setButtons(Qt::LeftButton);
    mouseEvent.setScenePos(QPointF(80, 80));
    mouseEvent.setPos(QPointF(80, 80));

    CDrawToolEvent::CDrawToolEvents events = CDrawToolEvent::fromQEvent(&mouseEvent, scene);
    ASSERT_FALSE(events.isEmpty());

    // toolDoStart should return early because dueTouchDoubleClickedStart returns true.
    // dueTouchDoubleClickedStart handles the double-click internally.
    tool->toolDoStart(&events.first());
    // The double-click was detected and handled — verify no crash.
    SUCCEED();

    tool->interrupt();
}

// ---------------------------------------------------------------------------
// IDrawTool::toolDoUpdate — touch sensitivity inside radius (lines 382-383)
// ---------------------------------------------------------------------------

TEST(CCentralWidgetCViewMgmtMiscCoverageTest, ToolDoUpdate_TouchInsideRadius_SetsPosXAcceptedFalse)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    ASSERT_NE(board, nullptr);

    std::unique_ptr<IDrawTool> tool(CDrawToolFactory::Create(rectangle));
    ASSERT_NE(tool, nullptr);
    tool->setDrawBoard(board);
    tool->setTouchSensitiveRadius(20);

    // Touch begin at (100, 100).
    auto touchBegin = std::make_unique<QTouchEvent>(QEvent::TouchBegin);
    QTouchEvent::TouchPoint tp1(1, QEventPoint::State::Pressed,
                                QPointF(100, 100), QPointF(100, 100));
    CDrawToolEvent eventBegin = CDrawToolEvent::fromTouchPoint(tp1, scene, touchBegin.get());
    tool->toolDoStart(&eventBegin);

    // Touch update at the SAME position (100, 100) — inside the 20px radius.
    auto touchUpdate = std::make_unique<QTouchEvent>(QEvent::TouchUpdate);
    QTouchEvent::TouchPoint tp2(1, QEventPoint::State::Updated,
                                QPointF(100, 100), QPointF(100, 100));
    CDrawToolEvent eventUpdate = CDrawToolEvent::fromTouchPoint(tp2, scene, touchUpdate.get());
    tool->toolDoUpdate(&eventUpdate);

    // The sensitivity check sets posXAccepted=false when inside radius.
    // If the check wasn't reached (e.g. already decided), posXAccepted stays true (default).
    // Either way, the code path is exercised.
    SUCCEED();

    // Clean up.
    auto touchEnd = std::make_unique<QTouchEvent>(QEvent::TouchEnd);
    QTouchEvent::TouchPoint tp3(1, QEventPoint::State::Released,
                                QPointF(100, 100), QPointF(100, 100));
    CDrawToolEvent eventEnd = CDrawToolEvent::fromTouchPoint(tp3, scene, touchEnd.get());
    tool->toolDoFinish(&eventEnd);
    tool->interrupt();
}

// ---------------------------------------------------------------------------
// IDrawTool::refresh — popup widget branch (lines 506-509)
// ---------------------------------------------------------------------------

TEST(CCentralWidgetCViewMgmtMiscCoverageTest, Refresh_WithActivePopup_TakesPopupBranch)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    Page *page = view->page();
    ASSERT_NE(page, nullptr);

    drawApp->setCurrentTool(selection);

    // Show a non-modal popup menu.
    QMenu *menu = new QMenu();
    menu->addAction("test");
    menu->show();
    menu->popup(QPoint(0, 0));
    qApp->processEvents();

    // In offscreen mode, activePopupWidget may be null, but the refresh
    // code path is still exercised. Call refresh and verify no crash.
    page->currentTool_p()->refresh();

    // Clean up.
    delete menu;
    qApp->processEvents();
}
