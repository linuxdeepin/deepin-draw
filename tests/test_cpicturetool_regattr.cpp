// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QPushButton>

#include "publicApi.h"

#define protected public
#define private public
#include "globaldefine.h"
#include "cdrawtoolfactory.h"
#include "cpicturetool.h"
#include "cattributemanagerwgt.h"
#include "cattributeitemwidget.h"   // DrawAttribution::EImageLeftRot etc.

/*
 * CPictureTool::registerAttributionWidgets (142 lines, complexity 10)
 *
 * Creates five QPushButtons (left rotate, right rotate, flip-H, flip-V,
 * auto-fit) with specific object names, connects their clicked signals to
 * drawBoard()->setDrawAttribution(...), and installs them via
 * drawBoard()->attributionWidget()->installComAttributeWgt(...).
 *
 * installComAttributeWgt stores each widget in the member map
 * s_allInstalledAttriWgts (keyed by attribution enum). We verify all five
 * EImage* keys are present after the call.
 */

TEST(CPictureToolRegisterAttributionTest, InstallsFiveAttributeButtons)
{
    createNewViewByShortcutKey();
    drawApp->setCurrentTool(picture);

    CPictureTool *picTool = dynamic_cast<CPictureTool *>(
        CDrawToolFactory::tool(picture));
    ASSERT_NE(picTool, nullptr);

    auto *attrWgt = drawApp->topMainWindow()->drawBoard()->attributionWidget();
    ASSERT_NE(attrWgt, nullptr);

    picTool->registerAttributionWidgets();

    // All five image-attribution keys should be installed in the map.
    using namespace DrawAttribution;
    EXPECT_TRUE(attrWgt->s_allInstalledAttriWgts.contains(EImageLeftRot));
    EXPECT_TRUE(attrWgt->s_allInstalledAttriWgts.contains(EImageRightRot));
    EXPECT_TRUE(attrWgt->s_allInstalledAttriWgts.contains(EImageHorFilp));
    EXPECT_TRUE(attrWgt->s_allInstalledAttriWgts.contains(EImageVerFilp));
    EXPECT_TRUE(attrWgt->s_allInstalledAttriWgts.contains(EImageAdaptScene));

    // The installed widgets should be QPushButtons with the expected names.
    auto *leftBtn = qobject_cast<QPushButton *>(
        attrWgt->s_allInstalledAttriWgts.value(EImageLeftRot));
    ASSERT_NE(leftBtn, nullptr);
    EXPECT_EQ(leftBtn->objectName(), QStringLiteral("PicLeftRotateBtn"));
}
