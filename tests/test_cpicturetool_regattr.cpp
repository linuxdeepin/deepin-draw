// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QPushButton>
#include <QSet>

#include "publicApi.h"

#define protected public
#define private public
#include "globaldefine.h"
#include "cdrawtoolfactory.h"
#include "cpicturetool.h"
#include "cattributemanagerwgt.h"
#include "cattributeitemwidget.h"   // DrawAttribution::EImageLeftRot etc.

// Snapshot of the board-wide shared attribute registry. Calling
// registerAttributionWidgets() on the tool replaces the entries the
// application registered at startup (DrawBoard::initTools) with freshly
// created buttons; this restores them and deletes the orphaned
// test-created widgets on scope exit.
class AttrRegistrySnapshot
{
public:
    explicit AttrRegistrySnapshot(CAttributeManagerWgt *mgr)
        : m_mgr(mgr), m_wgts(mgr->s_allInstalledAttriWgts),
          m_vars(mgr->s_allInstalledDefaultVar) {}

    ~AttrRegistrySnapshot()
    {
        QSet<QWidget *> preWidgets;
        for (QWidget *w : m_wgts)
            preWidgets.insert(w);
        for (auto it = m_mgr->s_allInstalledAttriWgts.constBegin();
             it != m_mgr->s_allInstalledAttriWgts.constEnd(); ++it) {
            if (!preWidgets.contains(it.value()) && it.value()->parent() == nullptr)
                delete it.value();
        }
        m_mgr->s_allInstalledAttriWgts = m_wgts;
        m_mgr->s_allInstalledDefaultVar = m_vars;
    }

private:
    CAttributeManagerWgt *m_mgr;
    QMap<int, QWidget *> m_wgts;
    QMap<int, QVariant> m_vars;
};

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

    // The factory map may hold a freshly created instance without a
    // board (e.g. after Application::loadTools() ran in an earlier
    // test); bind the tool to the live board so
    // registerAttributionWidgets does not dereference a null board.
    picTool->setDrawBoard(drawApp->topMainWindow()->drawBoard());

    auto *attrWgt = drawApp->topMainWindow()->drawBoard()->attributionWidget();
    ASSERT_NE(attrWgt, nullptr);

    // registerAttributionWidgets() below replaces the app-registered
    // EImage* entries with fresh buttons — restore them on scope exit.
    AttrRegistrySnapshot attrSnap(attrWgt);

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
