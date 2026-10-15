// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>
#include <QApplication>
#include <QColor>
#include <QFont>
#include <QImage>
#include <QPainter>
#include <QPainterPath>
#include <QTextCursor>
#include <QTextEdit>
#include <memory>

#define protected public
#define private public
#include "widgets/ctextedit.h"
#include "cgraphicsproxywidget.h"
#include "drawshape/drawItems/bzItems/cgraphicstextitem.h"
#include "publicApi.h"
#undef protected
#undef private

/*
 * Branch list (from MCP source snippets):
 * B1: changToEditState - multi-selected/group -> return.
 * B2: changToEditState - no scene -> selection tool and local edit state.
 * B3: changToEditState - selectAll=true -> document cursor selection.
 * B4: changToEditState - selectAll=false -> cursor moved to end.
 * B5: changToReadOnlyState - editor exists -> set ItemHasNoContents.
 * B6: setTextState - same state -> early return.
 * B7: setTextState - EInEdit -> edit path.
 * B8: setTextState - non-EInEdit -> read-only path.
 * B9: beginPreview - first call -> begin edit block.
 * B10: beginPreview - repeated call -> join edit block.
 * B11: endPreview - revert with selection -> undo.
 * B12: endPreview - no revert -> only close preview.
 * B13: updateToDefaultTextFormat - no page -> keep current text.
 * B14: setSelectTextBlockAlign - all alignment cases.
 * These branches are mapped by the test names below.
 */

class CGraphicsTextItemUnitTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        // Arrange
        item = new CGraphicsTextItem(QStringLiteral("Hello draw"));
        ASSERT_NE(item, nullptr);
        ASSERT_NE(item->m_pTextEdit, nullptr);
        ASSERT_NE(item->m_pProxy, nullptr);
    }

    void TearDown() override
    {
        if (item != nullptr) {
            delete item->m_pTextEdit;
            item->m_pTextEdit = nullptr;
            delete item;
            item = nullptr;
        }
    }

    CGraphicsTextItem *item = nullptr;
};

TEST_F(CGraphicsTextItemUnitTest, Constructor_Initializes_EditorState)
{
    // Arrange
    const QString expectedText = QStringLiteral("Hello draw");

    // Act
    CTextEdit *editor = item->textEditor();

    // Assert
    EXPECT_EQ(editor->toPlainText(), expectedText);
    EXPECT_EQ(editor->objectName(), QStringLiteral("TextEditor"));
    EXPECT_EQ(item->textState(), CGraphicsTextItem::EReadOnly);
}

TEST_F(CGraphicsTextItemUnitTest, Attributions_Expose_CurrentValues)
{
    // Arrange
    item->setFontFamily(QStringLiteral("Sans Serif"));
    item->setFontStyle(QStringLiteral("Bold"));
    item->setFontSize(18);
    const QColor expectedColor(Qt::blue);

    // Act
    item->setTextColor(expectedColor);
    const auto attrList = item->attributions();

    // Assert
    EXPECT_EQ(attrList.size(), 4);
    EXPECT_EQ(attrList.first().var, expectedColor);
    EXPECT_EQ(item->fontFamily(), QStringLiteral("Sans Serif"));
    EXPECT_EQ(item->fontStyle(), QStringLiteral("Bold"));
    EXPECT_EQ(item->fontSize(), 18);
}

TEST_F(CGraphicsTextItemUnitTest, Font_Setters_GettersRoundTrip)
{
    // Arrange
    QFont font;
    font.setFamily(QStringLiteral("DejaVu Sans"));
    font.setBold(true);

    // Act
    item->setFont(font);
    item->setFontStyle(QStringLiteral("Bold"));
    item->setFontSize(21);
    item->setFontFamily(QStringLiteral("DejaVu Sans"));

    // Assert
    EXPECT_EQ(item->fontFamily(), QStringLiteral("DejaVu Sans"));
    EXPECT_EQ(item->fontStyle(), QStringLiteral("Bold"));
    EXPECT_EQ(item->fontSize(), 21);
}

TEST_F(CGraphicsTextItemUnitTest, Highlight_Path_MatchesItemRect)
{
    // Arrange
    const QRectF expectedRect(2, 3, 40, 20);
    item->setRect(expectedRect);

    // Act
    const QPainterPath path = item->getHighLightPath();

    // Assert
    EXPECT_EQ(path.boundingRect(), expectedRect);
    EXPECT_FALSE(path.isEmpty());
}

TEST_F(CGraphicsTextItemUnitTest, Edit_Transition_ReadOnlyAfterChange)
{
    // Arrange
    ASSERT_NE(item, nullptr);

    // Act
    item->changToEditState(true);
    const bool editAfterChange = item->isEditState();

    // Assert
    EXPECT_TRUE(editAfterChange);
    EXPECT_TRUE(item->m_pTextEdit->textCursor().hasSelection());

    // Act
    item->changToReadOnlyState(true);

    // Assert
    EXPECT_EQ(item->textState(), CGraphicsTextItem::EReadOnly);
    EXPECT_FALSE(item->isEditState());
}

TEST_F(CGraphicsTextItemUnitTest, SetText_SameState_IgnoresRequest)
{
    // Arrange
    ASSERT_EQ(item->textState(), CGraphicsTextItem::EReadOnly);
    const auto proxyFlagsBefore = item->m_pProxy->flags();

    // Act
    item->setTextState(CGraphicsTextItem::EReadOnly, false);
    const auto proxyFlagsAfter = item->m_pProxy->flags();

    // Assert
    EXPECT_EQ(proxyFlagsBefore, proxyFlagsAfter);
    EXPECT_EQ(item->textState(), CGraphicsTextItem::EReadOnly);
}

TEST_F(CGraphicsTextItemUnitTest, SetText_NoSelection_MovesCursorEnd)
{
    // Arrange
    ASSERT_EQ(item->textState(), CGraphicsTextItem::EReadOnly);

    // Act
    item->setTextState(CGraphicsTextItem::EInEdit, false);

    // Assert
    EXPECT_EQ(item->textState(), CGraphicsTextItem::EInEdit);
    EXPECT_EQ(item->m_pTextEdit->textCursor().position(),
              item->m_pTextEdit->toPlainText().length());
}

TEST_F(CGraphicsTextItemUnitTest, Preview_BeginEnd_RevertsSelection)
{
    // Arrange
    ASSERT_FALSE(item->isPreview());

    // Act
    item->beginPreview();
    QTextCursor cursor = item->m_pTextEdit->textCursor();
    cursor.movePosition(QTextCursor::End);
    cursor.insertText(QStringLiteral("X"));
    cursor.select(QTextCursor::Document);
    item->m_pTextEdit->setTextCursor(cursor);

    // Assert
    EXPECT_TRUE(item->isPreview());

    // Act
    item->endPreview(true);

    // Assert
    EXPECT_FALSE(item->isPreview());
    EXPECT_EQ(item->m_pTextEdit->toPlainText(), QStringLiteral("Hello draw"));
}

TEST_F(CGraphicsTextItemUnitTest, Preview_Repeat_DoesNotRevert)
{
    // Arrange
    item->beginPreview();
    item->beginPreview();

    // Act
    item->endPreview(false);

    // Assert
    EXPECT_FALSE(item->isPreview());
    EXPECT_EQ(item->m_pTextEdit->toPlainText(), QStringLiteral("Hello draw"));
}

TEST_F(CGraphicsTextItemUnitTest, Proxy_Position_FollowsItemRect)
{
    // Arrange
    const QRectF geometry(7, 9, 40, 25);
    item->setRect(geometry);

    // Act
    item->updateProxyItemPos();

    // Assert
    EXPECT_EQ(item->m_pProxy->pos(), QPointF(geometry.x(), geometry.y()));
    EXPECT_EQ(item->m_pProxy->boundingRect().size().toSize(),
              QSize(geometry.width(), geometry.height()));
}

TEST_F(CGraphicsTextItemUnitTest, AutoAdjust_Size_ControlsWrapMode)
{
    // Arrange
    ASSERT_TRUE(item->isAutoAdjustSize());

    // Act
    item->setAutoAdjustSize(false);

    // Assert
    EXPECT_FALSE(item->isAutoAdjustSize());
    EXPECT_EQ(item->m_pTextEdit->lineWrapMode(), QTextEdit::WidgetWidth);

    // Act
    item->setAutoAdjustSize(true);

    // Assert
    EXPECT_TRUE(item->isAutoAdjustSize());
    EXPECT_EQ(item->m_pTextEdit->lineWrapMode(), QTextEdit::NoWrap);
}

TEST_F(CGraphicsTextItemUnitTest, UpdateDefault_NoPage_KeepsText)
{
    // Arrange
    const QString originalText = item->m_pTextEdit->toPlainText();

    // Act
    item->updateToDefaultTextFormat();

    // Assert
    EXPECT_EQ(item->m_pTextEdit->toPlainText(), originalText);
    EXPECT_TRUE(item->m_pTextEdit->textCursor().hasSelection());
}

TEST_F(CGraphicsTextItemUnitTest, ToFocus_Editor_UpdatesEditState)
{
    // Arrange
    ASSERT_EQ(item->textState(), CGraphicsTextItem::EReadOnly);
    item->m_pTextEdit->setTextInteractionFlags(Qt::NoTextInteraction);

    // Act
    item->toFocusEiditor();
    const bool readOnlyFocusFlags = item->m_pTextEdit->textInteractionFlags()
                                    & Qt::TextEditable;

    // Assert
    EXPECT_EQ(readOnlyFocusFlags, 0);
    EXPECT_EQ(item->textState(), CGraphicsTextItem::EReadOnly);

    // Act
    item->changToEditState(false);
    item->toFocusEiditor();

    // Assert
    EXPECT_NE(item->m_pTextEdit->textInteractionFlags() & Qt::TextEditable, 0);
    EXPECT_EQ(item->textState(), CGraphicsTextItem::EInEdit);
}

TEST_F(CGraphicsTextItemUnitTest, UndoRedo_Delete_OperateEditor)
{
    // Arrange
    item->changToEditState(false);
    item->m_pTextEdit->document()->clearUndoRedoStacks();
    const QString originalText = item->m_pTextEdit->toPlainText();
    QTextCursor cursor = item->m_pTextEdit->textCursor();
    cursor.movePosition(QTextCursor::End);
    cursor.movePosition(QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor);
    item->m_pTextEdit->setTextCursor(cursor);

    // Act
    item->doDelete();
    const QString deletedText = item->m_pTextEdit->toPlainText();

    // Assert
    EXPECT_EQ(deletedText, originalText.left(originalText.length() - 1));

    // Act
    item->doUndo();
    const QString undoneText = item->m_pTextEdit->toPlainText();

    // Assert
    EXPECT_EQ(undoneText, originalText);

    // Act
    item->doRedo();

    // Assert
    EXPECT_EQ(item->m_pTextEdit->toPlainText(), deletedText);
}

TEST_F(CGraphicsTextItemUnitTest, SelectAlign_AllModes_UpdateAlignment)
{
    // Arrange
    ASSERT_NE(item, nullptr);

    // Act
    item->setSelectTextBlockAlign(Qt::AlignLeft);

    // Assert
    EXPECT_EQ(item->m_pTextEdit->alignment(), Qt::AlignLeft);

    item->setSelectTextBlockAlign(Qt::AlignRight);
    EXPECT_EQ(item->m_pTextEdit->alignment(), Qt::AlignRight);

    item->setSelectTextBlockAlign(Qt::AlignHCenter);
    EXPECT_EQ(item->m_pTextEdit->alignment(), Qt::AlignHCenter);

    item->setSelectTextBlockAlign(Qt::AlignTop);
    EXPECT_EQ(item->m_pTextEdit->alignment(), Qt::AlignTop);

    item->setSelectTextBlockAlign(Qt::AlignBottom);
    EXPECT_EQ(item->m_pTextEdit->alignment(), Qt::AlignBottom);

    item->setSelectTextBlockAlign(Qt::AlignVCenter);
    EXPECT_EQ(item->m_pTextEdit->alignment(), Qt::AlignVCenter);

    item->setSelectTextBlockAlign(Qt::AlignCenter);
    EXPECT_EQ(item->m_pTextEdit->alignment(), Qt::AlignCenter);
}

TEST_F(CGraphicsTextItemUnitTest, GraphicsUnit_SaveLoad_RestoresAttributes)
{
    // Arrange
    const QColor color(Qt::darkGreen);
    item->setRect(QRectF(4, 5, 80, 35));
    item->setFontFamily(QStringLiteral("DejaVu Sans"));
    item->setFontStyle(QStringLiteral("Bold"));
    item->setFontSize(23);
    item->setTextColor(color);
    item->setAutoAdjustSize(false);

    // Act
    CGraphicsUnit unit = item->getGraphicsUnit(EDuplicate);
    ASSERT_NE(unit.data.pText, nullptr);

    CGraphicsTextItem dest(QStringLiteral("different"));
    dest.loadGraphicsUnit(unit);

    // Assert
    EXPECT_EQ(dest.rect(), item->rect());
    EXPECT_EQ(dest.fontFamily(), item->fontFamily());
    EXPECT_EQ(dest.fontStyle(), item->fontStyle());
    EXPECT_EQ(dest.fontSize(), item->fontSize());
    EXPECT_EQ(dest.textColor(), color);
    EXPECT_EQ(dest.isAutoAdjustSize(), false);
    unit.release();
}
