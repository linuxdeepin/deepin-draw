// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for CTextEdit::inputMethodEvent (issue V-7429)
 *
 * Source: src/widgets/ctextedit.cpp
 *
 * Target method:
 *   inputMethodEvent(QInputMethodEvent *e) override
 *
 * When commitString is non-empty, the event is forwarded to
 * QTextEdit::inputMethodEvent. When empty, nothing happens.
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QInputMethodEvent>
#include <QString>
#include <QApplication>

#define protected public
#define private public
#include "ctextedit.h"
#include "drawshape/drawItems/bzItems/cgraphicstextitem.h"
#undef protected
#undef private

TEST(CTextEditInputMethodEvent, EmptyCommitStringDoesNotCrash)
{
    CGraphicsTextItem item(QStringLiteral("test"));
    CTextEdit edit(&item);
    QInputMethodEvent event;
    event.setCommitString("");
    QApplication::sendEvent(&edit, &event);
    SUCCEED();
}

TEST(CTextEditInputMethodEvent, NonEmptyCommitStringUpdatesText)
{
    CGraphicsTextItem item(QStringLiteral("test"));
    CTextEdit edit(&item);
    QInputMethodEvent event;
    event.setCommitString("hello");
    QApplication::sendEvent(&edit, &event);
    // The commit string should appear in the text edit's content.
    EXPECT_TRUE(edit.toPlainText().contains("hello"));
}

TEST(CTextEditInputMethodEvent, MultipleCommitsAppendText)
{
    CGraphicsTextItem item(QStringLiteral("test"));
    CTextEdit edit(&item);
    QInputMethodEvent event1;
    event1.setCommitString("abc");
    QApplication::sendEvent(&edit, &event1);

    QInputMethodEvent event2;
    event2.setCommitString("def");
    QApplication::sendEvent(&edit, &event2);

    EXPECT_TRUE(edit.toPlainText().contains("abc"));
    EXPECT_TRUE(edit.toPlainText().contains("def"));
}

TEST(CTextEditInputMethodEvent, PreeditStringOnlyDoesNotCommit)
{
    CGraphicsTextItem item(QStringLiteral("test"));
    CTextEdit edit(&item);
    // Construct with preedit string - no commit string
    QInputMethodEvent event(QStringLiteral("preedit"), QList<QInputMethodEvent::Attribute>());
    QApplication::sendEvent(&edit, &event);
    // preeditString should not be committed to plain text
    EXPECT_FALSE(edit.toPlainText().contains("preedit"));
}
