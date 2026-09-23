// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QList>
#include <QString>
#include <QStringList>

#define protected public
#define private public
#include "shortcut.h"

/*
 * The Shortcut constructor assembles five hard-coded shortcut groups (Files,
 * Drawing, Shapes/Images, Settings, Align), each populated with a fixed set of
 * ShortcutItem(name, value) pairs, and finally mirrors the whole structure into
 * the private m_shortcutObj JSON document. These tests pin that contract.
 */

// Verifies the five groups exist, in order, with the expected names and item
// counts, and spot-checks a few representative items.
TEST(ShortcutCtorTest, BuildsFiveGroupsWithExpectedNamesAndCounts)
{
    Shortcut shortcut;

    ASSERT_EQ(shortcut.m_shortcutGroups.size(), 5);

    const QStringList expectedNames{
        Shortcut::tr("Files"),
        Shortcut::tr("Drawing"),
        Shortcut::tr("Shapes/Images"),
        Shortcut::tr("Settings"),
        Shortcut::tr("Align")
    };
    const QList<int> expectedCounts{6, 15, 12, 2, 6};

    for (int i = 0; i < expectedNames.size(); ++i) {
        SCOPED_TRACE(::testing::Message() << "group index " << i);
        EXPECT_EQ(shortcut.m_shortcutGroups.at(i).groupName, expectedNames.at(i));
        EXPECT_EQ(shortcut.m_shortcutGroups.at(i).groupItems.size(), expectedCounts.at(i));
    }

    // "Files" group: first item (New -> Ctrl+N) and a mid item (Save as -> Ctrl+Shift+S).
    const auto &files = shortcut.m_shortcutGroups.at(0).groupItems;
    ASSERT_GE(files.size(), 5);
    EXPECT_EQ(files.at(0).name, Shortcut::tr("New"));
    EXPECT_EQ(files.at(0).value, QStringLiteral("Ctrl+N"));
    EXPECT_EQ(files.at(4).name, Shortcut::tr("Save as"));
    EXPECT_EQ(files.at(4).value, QStringLiteral("Ctrl+Shift+S"));

    // "Settings" group (2 items): Help(F1) + Display shortcuts(Ctrl + Shift + ?).
    const auto &settings = shortcut.m_shortcutGroups.at(3).groupItems;
    ASSERT_EQ(settings.size(), 2);
    EXPECT_EQ(settings.at(0).name, Shortcut::tr("Help"));
    EXPECT_EQ(settings.at(0).value, QStringLiteral("F1"));
    EXPECT_EQ(settings.at(1).name, Shortcut::tr("Display shortcuts"));
    EXPECT_EQ(settings.at(1).value, QStringLiteral("Ctrl + Shift + ?"));
}

// Verifies the JSON object produced in the ctor mirrors m_shortcutGroups: same
// group count, per-group item count, and matching name/value fields.
TEST(ShortcutCtorTest, JsonObjectMirrorsShortcutGroups)
{
    Shortcut shortcut;

    ASSERT_TRUE(shortcut.m_shortcutObj.contains("shortcut"));
    const QJsonArray jsonGroups = shortcut.m_shortcutObj.value("shortcut").toArray();
    ASSERT_EQ(jsonGroups.size(), shortcut.m_shortcutGroups.size());

    for (int i = 0; i < shortcut.m_shortcutGroups.size(); ++i) {
        SCOPED_TRACE(::testing::Message() << "group index " << i);
        const auto &group = shortcut.m_shortcutGroups.at(i);
        const QJsonObject jsonGroup = jsonGroups.at(i).toObject();
        EXPECT_EQ(jsonGroup.value("groupName").toString(), group.groupName);

        const QJsonArray jsonItems = jsonGroup.value("groupItems").toArray();
        EXPECT_EQ(jsonItems.size(), group.groupItems.size());
        for (int j = 0; j < group.groupItems.size(); ++j) {
            SCOPED_TRACE(::testing::Message() << "item index " << j);
            const QJsonObject jsonItem = jsonItems.at(j).toObject();
            EXPECT_EQ(jsonItem.value("name").toString(), group.groupItems.at(j).name);
            EXPECT_EQ(jsonItem.value("value").toString(), group.groupItems.at(j).value);
        }
    }

    // The document is serialisable and non-empty.
    const QJsonDocument doc(shortcut.m_shortcutObj);
    EXPECT_FALSE(doc.isEmpty());
    EXPECT_FALSE(doc.toJson(QJsonDocument::Compact).isEmpty());
}
