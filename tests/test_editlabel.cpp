// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for EditLabel (issue V-6403).
 *
 * Source: src/widgets/editlabel.cpp
 *
 * Target methods:
 *   EditLabel(DWidget*), ~EditLabel()
 *
 * Verifies DLineEdit default properties set by the constructor:
 *   fixed size 55x36, clearButton disabled, readOnly, font 14px.
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QFont>
#include <QSize>
#include <QLineEdit>

#define protected public
#define private public
#include "widgets/editlabel.h"
#undef protected
#undef private

// ---- EditLabel constructors / destructor ----

TEST(EditLabelTest, ConstructDoesNotCrash)
{
    EditLabel label;
    EXPECT_EQ(label.size(), QSize(55, 36));
}

TEST(EditLabelTest, ConstructWithParent)
{
    DWidget parent;
    EditLabel label(&parent);
    EXPECT_EQ(label.parentWidget(), &parent);
}

TEST(EditLabelTest, FixedSizeIsCorrect)
{
    EditLabel label;
    EXPECT_EQ(label.minimumSize(), QSize(55, 36));
    EXPECT_EQ(label.maximumSize(), QSize(55, 36));
}

TEST(EditLabelTest, ClearButtonIsDisabled)
{
    EditLabel label;
    EXPECT_FALSE(label.isClearButtonEnabled());
}

TEST(EditLabelTest, LineEditIsReadOnly)
{
    EditLabel label;
    EXPECT_TRUE(label.lineEdit()->isReadOnly());
}

TEST(EditLabelTest, FontPixelSizeIs14)
{
    EditLabel label;
    EXPECT_EQ(label.font().pixelSize(), 14);
}

TEST(EditLabelTest, DestructorDoesNotCrash)
{
    {
        EditLabel *label = new EditLabel;
        delete label;
    }
    SUCCEED();
}
