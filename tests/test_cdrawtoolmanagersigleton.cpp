// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for CDrawToolManagerSigleton::getDrawTool (issue V-5575)
 *
 * Source: src/drawshape/drawTools/cdrawtoolmanagersigleton.cpp
 *
 * Target method:
 *   CDrawToolManagerSigleton::getDrawTool
 *
 * getDrawTool delegates to CDrawToolFactory::tool(mode), which looks
 * up the static map s_tools. Tools must be created via Create() and
 * installed via installTool() before they can be retrieved.
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#define protected public
#define private public
#include "drawshape/drawTools/cdrawtoolmanagersigleton.h"
#include "drawshape/drawTools/cdrawtoolfactory.h"
#include "drawshape/drawTools/idrawtool.h"
#include "drawshape/globaldefine.h"
#undef protected
#undef private

// Creates and installs a tool for `mode`, then on scope exit restores the
// prior process-wide factory entry (or removes the key if none existed)
// and deletes the created tool, so tests leave s_tools untouched.
class InstalledToolGuard
{
public:
    explicit InstalledToolGuard(EDrawToolMode mode)
        : m_mode(static_cast<int>(mode))
    {
        CDrawToolFactory::CDrawToolsMap &tools = CDrawToolFactory::allTools();
        const auto it = tools.find(m_mode);
        if (it != tools.end()) {
            m_hadPrevious = true;
            m_previous = it.value();
        }
        m_tool = CDrawToolFactory::Create(mode);
        CDrawToolFactory::installTool(m_tool);
    }

    ~InstalledToolGuard()
    {
        CDrawToolFactory::CDrawToolsMap &tools = CDrawToolFactory::allTools();
        if (m_hadPrevious)
            tools.insert(m_mode, m_previous);
        else
            tools.remove(m_mode);
        delete m_tool;
    }

    IDrawTool *tool() const { return m_tool; }

private:
    int m_mode;
    bool m_hadPrevious = false;
    IDrawTool *m_previous = nullptr;
    IDrawTool *m_tool = nullptr;
};

// =========================================================================
// CDrawToolManagerSigleton::GetInstance
// =========================================================================

TEST(CDrawToolManagerSigletonTest, GetInstanceReturnsNonNull)
{
    auto *singleton = CDrawToolManagerSigleton::GetInstance();
    EXPECT_NE(singleton, nullptr);
}

TEST(CDrawToolManagerSigletonTest, GetInstanceReturnsSameSingleton)
{
    auto *singleton1 = CDrawToolManagerSigleton::GetInstance();
    auto *singleton2 = CDrawToolManagerSigleton::GetInstance();
    EXPECT_EQ(singleton1, singleton2);
}

// =========================================================================
// CDrawToolManagerSigleton::getDrawTool
// =========================================================================

TEST(CDrawToolManagerSigletonGetDrawToolTest, UnknownModeReturnsNull)
{
    auto *singleton = CDrawToolManagerSigleton::GetInstance();
    IDrawTool *tool = singleton->getDrawTool(
        static_cast<EDrawToolMode>(999));
    EXPECT_EQ(tool, nullptr);
}

TEST(CDrawToolManagerSigletonGetDrawToolTest, SelectionReturnsInstalledTool)
{
    InstalledToolGuard guard(selection);
    ASSERT_NE(guard.tool(), nullptr);

    auto *singleton = CDrawToolManagerSigleton::GetInstance();
    IDrawTool *tool = singleton->getDrawTool(selection);
    EXPECT_NE(tool, nullptr);
}

TEST(CDrawToolManagerSigletonGetDrawToolTest, RectangleReturnsInstalledTool)
{
    InstalledToolGuard guard(rectangle);
    ASSERT_NE(guard.tool(), nullptr);

    auto *singleton = CDrawToolManagerSigleton::GetInstance();
    IDrawTool *tool = singleton->getDrawTool(rectangle);
    ASSERT_NE(tool, nullptr);
    EXPECT_EQ(tool->getDrawToolMode(), rectangle);
}

TEST(CDrawToolManagerSigletonGetDrawToolTest, TextReturnsInstalledTool)
{
    InstalledToolGuard guard(text);
    ASSERT_NE(guard.tool(), nullptr);

    auto *singleton = CDrawToolManagerSigleton::GetInstance();
    IDrawTool *tool = singleton->getDrawTool(text);
    ASSERT_NE(tool, nullptr);
    EXPECT_EQ(tool->getDrawToolMode(), text);
}

TEST(CDrawToolManagerSigletonGetDrawToolTest, PenReturnsInstalledTool)
{
    InstalledToolGuard guard(pen);
    ASSERT_NE(guard.tool(), nullptr);

    auto *singleton = CDrawToolManagerSigleton::GetInstance();
    IDrawTool *tool = singleton->getDrawTool(pen);
    ASSERT_NE(tool, nullptr);
    EXPECT_EQ(tool->getDrawToolMode(), pen);
}

TEST(CDrawToolManagerSigletonGetDrawToolTest, DelegatesToFactoryTool)
{
    // Verify getDrawTool returns the same pointer as
    // CDrawToolFactory::tool for an installed tool.
    InstalledToolGuard guard(ellipse);
    ASSERT_NE(guard.tool(), nullptr);

    auto *singleton = CDrawToolManagerSigleton::GetInstance();
    IDrawTool *fromSingleton = singleton->getDrawTool(ellipse);
    IDrawTool *fromFactory = CDrawToolFactory::tool(ellipse);
    EXPECT_EQ(fromSingleton, fromFactory);
}
