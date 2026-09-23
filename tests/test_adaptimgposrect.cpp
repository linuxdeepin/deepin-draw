// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QImage>
#include <QPointF>
#include <QRectF>
#include <QString>

#include "publicApi.h"  // getCurView, createNewViewByShortcutKey, PageView, PageScene

/*
 * adaptImgPosAndRect() (defined in ccentralwidget.cpp) decides how an image
 * larger than the canvas is placed. It is an external free function declared
 * only inside cgraphicsview.cpp, so re-declare it here to link against it.
 *
 * Branches exercised (the choice == -1 branch pops a modal dialog and is
 * intentionally avoided to keep the tests deterministic):
 *   - null scene                         -> false, out-params untouched
 *   - image fits canvas                  -> true,  out-params untouched
 *   - exceeds + choice == 1 (auto fit)   -> true,  rect scaled, pos centred
 *   - exceeds + choice == 0 (keep orig)  -> true,  out-params untouched
 *   - exceeds + choice == -2 (cancel)    -> false
 */
extern bool adaptImgPosAndRect(PageScene *pScene, const QString &imgName,
                               const QImage &img, QPointF &pos, QRectF &rect, int &choice);

TEST(AdaptImgPosAndRectTest, NullSceneReturnsFalseAndLeavesOutputsUntouched)
{
    QPointF pos(7, 8);
    QRectF rect(1, 2, 3, 4);
    int choice = 1;
    QImage img(100, 100, QImage::Format_RGB32);

    EXPECT_FALSE(adaptImgPosAndRect(nullptr, QStringLiteral("x"), img, pos, rect, choice));
    EXPECT_EQ(pos, QPointF(7, 8));
    EXPECT_EQ(rect, QRectF(1, 2, 3, 4));
}

TEST(AdaptImgPosAndRectTest, ImageFitsCanvasReturnsTrueUnchanged)
{
    createNewViewByShortcutKey();
    PageScene *scene = getCurView()->drawScene();
    ASSERT_NE(scene, nullptr);
    scene->setSceneRect(QRectF(0, 0, 800, 600));

    QPointF pos(123, 456);
    QRectF rect(10, 20, 30, 40);
    int choice = 1;
    QImage img(100, 100, QImage::Format_RGB32);
    img.fill(Qt::red);

    EXPECT_TRUE(adaptImgPosAndRect(scene, QStringLiteral("img"), img, pos, rect, choice));
    EXPECT_EQ(pos, QPointF(123, 456));
    EXPECT_EQ(rect, QRectF(10, 20, 30, 40));
}

TEST(AdaptImgPosAndRectTest, ExceedsChoiceAutoFitScalesAndCenters)
{
    createNewViewByShortcutKey();
    PageScene *scene = getCurView()->drawScene();
    ASSERT_NE(scene, nullptr);
    scene->setSceneRect(QRectF(0, 0, 200, 200));

    QPointF pos(0, 0);
    QRectF rect(0, 0, 0, 0);
    int choice = 1;
    QImage img(400, 400, QImage::Format_RGB32);
    img.fill(Qt::blue);

    EXPECT_TRUE(adaptImgPosAndRect(scene, QStringLiteral("big"), img, pos, rect, choice));
    // scaleRatio = min(200/400, 200/400) = 0.5 -> rect (0,0,200,200)
    EXPECT_EQ(rect, QRectF(0, 0, 200, 200));
    // pos = sceneRect.center() - rect.center() = (100,100) - (100,100) = (0,0)
    EXPECT_EQ(pos, QPointF(0, 0));
}

TEST(AdaptImgPosAndRectTest, ExceedsChoiceKeepOriginalReturnsTrueUnchanged)
{
    createNewViewByShortcutKey();
    PageScene *scene = getCurView()->drawScene();
    ASSERT_NE(scene, nullptr);
    scene->setSceneRect(QRectF(0, 0, 200, 200));

    QPointF pos(9, 9);
    QRectF rect(5, 6, 7, 8);
    int choice = 0;
    QImage img(400, 400, QImage::Format_RGB32);
    img.fill(Qt::green);

    EXPECT_TRUE(adaptImgPosAndRect(scene, QStringLiteral("big"), img, pos, rect, choice));
    EXPECT_EQ(pos, QPointF(9, 9));
    EXPECT_EQ(rect, QRectF(5, 6, 7, 8));
}

TEST(AdaptImgPosAndRectTest, ExceedsChoiceCancelImportReturnsFalse)
{
    createNewViewByShortcutKey();
    PageScene *scene = getCurView()->drawScene();
    ASSERT_NE(scene, nullptr);
    scene->setSceneRect(QRectF(0, 0, 200, 200));

    QPointF pos(1, 1);
    QRectF rect(2, 2, 3, 3);
    int choice = -2;
    QImage img(400, 400, QImage::Format_RGB32);
    img.fill(Qt::cyan);

    EXPECT_FALSE(adaptImgPosAndRect(scene, QStringLiteral("big"), img, pos, rect, choice));
}
