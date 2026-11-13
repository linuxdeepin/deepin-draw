// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for Application lambdas
 * (src/application.cpp).
 *
 * Target lambdas (0-coverage):
 *   Application::execDraw — calls _dApp->exec() which blocks; skipped.
 *   ~Application — empty body; skipped (cannot destruct the global
 *       singleton).
 *   waitShowThenLoad lambda — the QMetaObject::invokeMethod lambda that
 *       calls actWin->loadFiles(paths).
 *
 * Why the load tests assert on the load RESULT instead of on modal
 * dialogs: DrawBoard::loadFiles() executes on a QtConcurrent worker thread
 * and drives the UI only through queued invocations, so a single
 * processEvents() dispatch can run its whole ProgressLayout lifecycle
 * (show -> delayClose() -> 200 ms timer -> close) and every harness
 * dismissed error dialog without the test thread ever observing them.
 * Polling QApplication::activeModalWidget() from the test body is
 * therefore unreliable — it can simply never see any modal. The
 * dependable observation is the load's state effect: a successfully
 * loaded image appears as a JDynamicLayer scene item, while files that
 * fail to load must add neither items nor pages.
 *
 * Why every non-empty load mixes in at least one valid image:
 * MainWindow::loadFiles() always forwards quitIfAllFialed=true to
 * DrawBoard::loadFiles() (src/frame/mainwindow.cpp), and an all-fail load
 * reaches that branch, which calls drawApp->quitApp() -> qApp->quit()
 * (src/frame/ccentralwidget.cpp) — an application-wide side effect no test
 * may trigger. With at least one file loaded successfully the branch is
 * provably not taken.
 *
 * The suite derives from PageStateGuard so pages are cleaned up, and each
 * load test removes the items its load added from the shared scene.
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QApplication>
#include <QElapsedTimer>
#include <QFileInfo>
#include <QImage>
#include <QStringList>
#include <QTemporaryDir>
#include <QTest>

#define protected public
#define private public
#include "application.h"
#include "ccentralwidget.h"
#include "cgraphicsview.h"
#include "drawshape/cdrawscene.h"
#include "drawshape/drawItems/bzItems/cgraphicsitem.h"
#include "publicApi.h"
#undef protected
#undef private

#include "test_helpers.h"

namespace {

// Polling step while pumping events for the background load to finish.
constexpr int kPumpStepMs = 20;
// Generous budget for a load to finish: the worker thread plus every
// harness-dismissed error dialog (about 300 ms each) plus the 200 ms
// deferred ProgressLayout close all happen inside this window.
constexpr int kLoadFinishTimeoutMs = 5000;
// A tiny image: smaller than the page scene so adaptImgPosAndRect() never
// needs to ask how to scale it, and far below the 10000-pixel guard.
constexpr int kTestImageWidth = 8;
constexpr int kTestImageHeight = 8;
// Paths that cannot exist, so FileHander::loadImage() fails cleanly with
// EFileNotExist.
const char kMissingImagePath[] = "/nonexistent/ut/missing_a.png";
const char kMissingImagePath2[] = "/nonexistent/ut/missing_b.jpg";

// Creates a small valid PNG inside dir and returns its full path, or an
// empty string when saving failed. The file stays readable while the
// QTemporaryDir lives.
QString makeValidImageFile(QTemporaryDir &dir)
{
    QImage img(kTestImageWidth, kTestImageHeight, QImage::Format_RGB32);
    img.fill(Qt::red);
    const QString path = dir.filePath(QStringLiteral("ut_load_image.png"));
    if (!img.save(path))
        return QString();
    return path;
}

// Pump the event loop until the background load becomes visible in the
// scene: at least one new bz item. Returns false on timeout. (The load's
// dialogs cannot be observed from here — see the file comment.)
bool pumpUntilItemAdded(PageScene *scene, int itemsBefore, int timeoutMs)
{
    QElapsedTimer timer;
    timer.start();
    while (timer.elapsed() < timeoutMs) {
        qApp->processEvents();
        if (scene->getBzItems().size() > itemsBefore)
            return true;
        QTest::qWait(kPumpStepMs);
    }
    return scene->getBzItems().size() > itemsBefore;
}

// Drain leftover modal dialogs after a successful load. The load's result
// is already proven by pumpUntilItemAdded; this only cleans up so the
// ProgressLayout (whose delayed close is a 200 ms singleShot that can fire
// after the item already appeared) is not left active for later tests.
// Returns true when no modal remains.
bool pumpUntilNoModal(int timeoutMs)
{
    QElapsedTimer timer;
    timer.start();
    while (QApplication::activeModalWidget() != nullptr
           && timer.elapsed() < timeoutMs)
        QTest::qWait(kPumpStepMs);
    return QApplication::activeModalWidget() == nullptr;
}

class ApplicationLambdaTest : public PageStateGuard
{
protected:
    DrawBoard *board()
    {
        DrawBoard *b = drawApp->drawBoard();
        EXPECT_NE(b, nullptr);
        return b;
    }

    PageScene *currentScene()
    {
        DrawBoard *b = board();
        EXPECT_NE(b, nullptr);
        Page *page = (b != nullptr) ? b->currentPage() : nullptr;
        EXPECT_NE(page, nullptr);
        PageView *view = (page != nullptr) ? page->view() : nullptr;
        EXPECT_NE(view, nullptr);
        return (view != nullptr) ? view->drawScene() : nullptr;
    }

    // Removes every item the load added from the shared current-page scene,
    // drops undo commands that hold raw item pointers, and restores the
    // page's dirty flag (adding an image marks the page modified).
    void removeLoadedItems(const QList<CGraphicsItem *> &itemsBefore, bool dirtyBefore)
    {
        DrawBoard *b = board();
        if (b == nullptr || b->currentPage() == nullptr)
            return;
        PageScene *s = currentScene();
        if (s == nullptr)
            return;
        s->clearSelectGroup();
        const QList<CGraphicsItem *> now = s->getBzItems();
        for (CGraphicsItem *item : now) {
            if (item != nullptr && !itemsBefore.contains(item))
                s->removeCItem(item, true);
        }
        PageView *view = b->currentPage()->view();
        if (view != nullptr && view->m_pUndoStack != nullptr)
            view->m_pUndoStack->clear();
        s->pageContext()->setDirty(dirtyBefore);
    }
};

} // namespace

// ===========================================================================
// Application::execDraw — blocks, skipped
// ===========================================================================

TEST_F(ApplicationLambdaTest, ExecDrawSkipped)
{
    GTEST_SKIP() << "Application::execDraw() calls _dApp->exec() which blocks "
                    "the event loop; cannot be tested.";
}

// ===========================================================================
// ~Application — empty body, trivial; singleton not destructible in tests
// ===========================================================================

TEST_F(ApplicationLambdaTest, DestructorIsTrivial)
{
    GTEST_SKIP() << "~Application() has an empty body and the global "
                    "singleton cannot be destructed within the test harness.";
}

// ===========================================================================
// waitShowThenLoad — empty paths add a blank page
// ===========================================================================

TEST_F(ApplicationLambdaTest, WaitShowThenLoadEmptyPaths_AddsBlankPage)
{
    DrawBoard *b = board();
    ASSERT_NE(b, nullptr);
    const int countBefore = b->count();

    // Empty paths -> addPage("") synchronously and returns immediately.
    drawApp->waitShowThenLoad(QStringList());
    qApp->processEvents();

    EXPECT_EQ(b->count(), countBefore + 1);
    ASSERT_NE(b->currentPage(), nullptr);
    // A blank page has no scene items.
    ASSERT_NE(b->currentPage()->view(), nullptr);
    EXPECT_TRUE(b->currentPage()->view()->drawScene()->getBzItems().isEmpty());
}

// ===========================================================================
// waitShowThenLoad — one valid image loads onto the current page
// ===========================================================================

TEST_F(ApplicationLambdaTest, WaitShowThenLoadValidImage_AddsImageToCurrentPage)
{
    DrawBoard *b = board();
    ASSERT_NE(b, nullptr);
    PageScene *s = currentScene();
    ASSERT_NE(s, nullptr);
    const int countBefore = b->count();
    const int itemsBefore = s->getBzItems().size();
    const bool dirtyBefore = s->pageContext()->isDirty();
    const QList<CGraphicsItem *> itemsBeforeList = s->getBzItems();

    QTemporaryDir dir;
    ASSERT_TRUE(dir.isValid());
    const QString imagePath = makeValidImageFile(dir);
    ASSERT_FALSE(imagePath.isEmpty());
    ASSERT_TRUE(QFileInfo::exists(imagePath));

    drawApp->waitShowThenLoad(QStringList() << imagePath);

    // The produced item proves the load's success path ran end to end.
    ASSERT_TRUE(pumpUntilItemAdded(s, itemsBefore, kLoadFinishTimeoutMs));
    // The load's ProgressLayout closes via a 200 ms singleShot that may not
    // have fired yet; drain it so no modal leaks into later tests.
    EXPECT_TRUE(pumpUntilNoModal(kLoadFinishTimeoutMs));

    // The image goes onto the CURRENT page: no page may be added.
    EXPECT_EQ(b->count(), countBefore);
    // Exactly one item was added — the loaded image, nothing else.
    EXPECT_EQ(s->getBzItems().size(), itemsBefore + 1);

    removeLoadedItems(itemsBeforeList, dirtyBefore);
    qApp->processEvents();
}

// ===========================================================================
// waitShowThenLoad — missing files are skipped, only the valid one loads
// ===========================================================================

TEST_F(ApplicationLambdaTest, WaitShowThenLoadMissingAndValidPaths_LoadsValidOnly)
{
    DrawBoard *b = board();
    ASSERT_NE(b, nullptr);
    PageScene *s = currentScene();
    ASSERT_NE(s, nullptr);
    const int countBefore = b->count();
    const int itemsBefore = s->getBzItems().size();
    const bool dirtyBefore = s->pageContext()->isDirty();
    const QList<CGraphicsItem *> itemsBeforeList = s->getBzItems();

    QTemporaryDir dir;
    ASSERT_TRUE(dir.isValid());
    const QString imagePath = makeValidImageFile(dir);
    ASSERT_FALSE(imagePath.isEmpty());
    ASSERT_TRUE(QFileInfo::exists(imagePath));

    // Two missing files plus one valid image. The missing ones are reported
    // and skipped without adding anything, and the successfully loaded one
    // keeps loaded==true so DrawBoard::loadFiles()' quitIfAllFialed branch
    // (drawApp->quitApp() -> qApp->quit()) is never entered — an all-fail
    // load would quit the whole application as a side effect.
    QStringList paths;
    paths << kMissingImagePath << kMissingImagePath2 << imagePath;
    drawApp->waitShowThenLoad(paths);

    ASSERT_TRUE(pumpUntilItemAdded(s, itemsBefore, kLoadFinishTimeoutMs));
    // See the valid-image test: drain any modal left by the load.
    EXPECT_TRUE(pumpUntilNoModal(kLoadFinishTimeoutMs));

    // Exactly one item (the valid image): the missing files added nothing,
    // and the load created no page.
    EXPECT_EQ(b->count(), countBefore);
    EXPECT_EQ(s->getBzItems().size(), itemsBefore + 1);

    removeLoadedItems(itemsBeforeList, dirtyBefore);
    qApp->processEvents();
}
