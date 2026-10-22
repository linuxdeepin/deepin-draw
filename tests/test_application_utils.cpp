// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QProcessEnvironment>
#include <Qt>

#define protected public
#define private public
#include "application.h"
#undef protected
#undef private

TEST(ApplicationUtils, setWidgetAccesibleNameSetsObjectName)
{
    QWidget w;
    Application::setWidgetAccesibleName(&w, "myWidget");
    EXPECT_EQ(w.objectName(), QString("myWidget"));
}

TEST(ApplicationUtils, setWidgetAccesibleNameNullptrNoCrash)
{
    Application::setWidgetAccesibleName(nullptr, "test");
    SUCCEED();
}

TEST(ApplicationUtils, setWidgetAccesibleNameEmpty)
{
    QWidget w;
    w.setObjectName("original");
    Application::setWidgetAccesibleName(&w, "");
    EXPECT_EQ(w.objectName(), QString(""));
}

TEST(ApplicationUtils, setWidgetAllPosterityNoFocusNullptrNoCrash)
{
    Application::setWidgetAllPosterityNoFocus(nullptr);
    SUCCEED();
}

TEST(ApplicationUtils, setWidgetAllPosterityNoFocusSetsChildren)
{
    QWidget parent;
    QWidget *child1 = new QWidget(&parent);
    QLabel *child2 = new QLabel(&parent);
    QWidget *grandchild = new QWidget(child1);

    child1->setFocusPolicy(Qt::StrongFocus);
    child2->setFocusPolicy(Qt::ClickFocus);
    grandchild->setFocusPolicy(Qt::WheelFocus);

    Application::setWidgetAllPosterityNoFocus(&parent);

    EXPECT_EQ(child1->focusPolicy(), Qt::NoFocus);
    EXPECT_EQ(child2->focusPolicy(), Qt::NoFocus);
    EXPECT_EQ(grandchild->focusPolicy(), Qt::NoFocus);
}

TEST(ApplicationUtils, setWidgetAllPosterityNoFocusNoChildren)
{
    QWidget w;
    w.setFocusPolicy(Qt::StrongFocus);
    Application::setWidgetAllPosterityNoFocus(&w);
    EXPECT_EQ(w.focusPolicy(), Qt::StrongFocus);
}

TEST(ApplicationUtils, isWaylandPlatformReturnsBool)
{
    bool result = Application::isWaylandPlatform();
    auto env = QProcessEnvironment::systemEnvironment();
    QString sessionType = env.value("XDG_SESSION_TYPE");
    QString waylandDisplay = env.value("WAYLAND_DISPLAY");

    if (sessionType == "wayland"
        || waylandDisplay.contains("wayland", Qt::CaseInsensitive)) {
        EXPECT_TRUE(result);
    } else {
        EXPECT_FALSE(result);
    }
}

TEST(ApplicationUtils, isTabletSystemEnvirReturnsFalse)
{
    bool result = Application::isTabletSystemEnvir();
    EXPECT_FALSE(result);
}

TEST(ApplicationUtils, isTabletSystemEnvirNoCrash)
{
    Application::isTabletSystemEnvir();
    SUCCEED();
}
