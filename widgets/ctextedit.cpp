/*
 * Copyright (C) 2019 ~ %YEAR% Deepin Technology Co., Ltd.
 *
 * Author:     WangXin
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "ctextedit.h"
#include "drawshape/cgraphicstextitem.h"
#include "drawshape/cdrawscene.h"
#include "drawshape/cgraphicsmasicoitem.h"
#include "frame/cviewmanagement.h"
#include "frame/cgraphicsview.h"

#include <DMenu>
#include <DApplication>
#include <QAction>
#include <QDebug>
#include <QTextBlock>
#include <QTextEdit>

CTextEdit::CTextEdit(CGraphicsTextItem *item, QWidget *parent)
    : QTextEdit(parent)
    , m_pItem(item)
    , m_widthF(0)
    , m_allColorIsEqual(true)
    , m_allSizeIsEqual(true)
    , m_allFamilyIsEqual(true)
    , m_allFontStyleIsEqual(true)
{
    //初始化字体
    connect(this, SIGNAL(textChanged()), this, SLOT(slot_textChanged()));

    connect(this, SIGNAL(cursorPositionChanged()),
            this, SLOT(cursorPositionChanged()));

    this->setLineWrapMode(NoWrap);
    this->setFrameStyle(NoFrame);
}

void CTextEdit::slot_textChanged()
{
    if (this->document()->isEmpty()) {
        m_pItem->setFont(CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getTextFont());
        m_pItem->setTextColor(CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getTextColor());
    }

    if (m_pItem->getManResizeFlag() || this->document()->lineCount() > 1) {
        this->setLineWrapMode(WidgetWidth);
    }

    QSizeF size = this->document()->size();
    QRectF rect = m_pItem->rect();
    rect.setHeight(size.height());
    rect.setWidth(size.width());

    //判断是否出界
    QPointF bottomRight = rect.bottomRight();
    QPointF bottomRightInScene = m_pItem->mapToScene(bottomRight);
    if (m_pItem->scene() != nullptr && !m_pItem->scene()->sceneRect().contains(bottomRightInScene)) {
        this->setLineWrapMode(WidgetWidth);
        this->document()->setTextWidth(m_widthF);
        size = this->document()->size();
        rect.setHeight(size.height());
        rect.setWidth(size.width());
    }

    if (m_pItem != nullptr) {
        m_pItem->setRect(rect);
    }

    m_widthF = rect.width();

    cursorPositionChanged();

    if (nullptr != m_pItem->scene()) {
        auto curScene = static_cast<CDrawScene *>(m_pItem->scene());
        //更新字图元
        curScene->updateBlurItem(m_pItem);
    }
}

void CTextEdit::cursorPositionChanged()
{
    // 只要鼠标点击后，此函数就会被调用一次
    if (this->document()->isEmpty()) {
        m_pItem->setFont(CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getTextFont());
        m_pItem->setTextColor(CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getTextColor());
    } else {
        QTextCursor cursor = this->textCursor();
        if (cursor.hasSelection()) {
            int startPos = cursor.selectionStart();
            int endPos = cursor.selectionEnd();
            qDebug() << "startpos = " << startPos << "endPos" << endPos;
            QTextBlock block = cursor.block();
            if (block.isValid()) {

                // [0] 检测选中文字的属性是否相等
                checkTextProperty(block);

                QTextBlock::iterator it;
                bool flag = true;
                QTextCharFormat chfFirst;
                QString fontFamily = "";
                QString fontStyle = "";
                for (it = block.begin(); !(it.atEnd()); ++it) {
                    QTextFragment fragment = it.fragment();
                    if (!fragment.isValid())
                        continue;
                    // 获取文本
                    QString text = fragment.text();
                    if (text.isEmpty())
                        continue;
                    // 获取文本格式
                    if (fragment.contains(startPos)) {
                        //第一次更改选中的字体和颜色
                        if (flag) {
                            chfFirst = fragment.charFormat();
                            fontFamily = chfFirst.font().family();
                            m_pItem->currentCharFormatChanged(chfFirst);
                            flag = false;

                            if (fragment.contains(endPos)) {
                                break;
                            }
                        }

                    } else {
                        if (flag) {
                            continue;
                        } else {
                            //找到
                            QTextCharFormat chf = fragment.charFormat();
                            //找到最后一个字块
                            if (fragment.contains(endPos)) {
                                if (endPos == fragment.position()) {
                                    break;
                                }
                                if (fontFamily != chf.font().family()) {
                                    m_pItem->currentCharFormatChanged(chfFirst);
                                    break;
                                }

                            } else {
                                if (fontFamily != chf.font().family()) {
                                    m_pItem->currentCharFormatChanged(chfFirst);
                                    break;
                                }
                            }

                        }
                    }

                }

            } else {
                m_pItem->currentCharFormatChanged(cursor.charFormat());
            }
        } else {
            m_pItem->currentCharFormatChanged(cursor.charFormat());
        }

        if (nullptr != m_pItem->scene()) {
            auto curScene = static_cast<CDrawScene *>(m_pItem->scene());
            curScene->updateBlurItem(m_pItem);
        }

        this->setFocus();
    }
}

void CTextEdit::checkTextProperty(QTextBlock block)
{
    m_allColorIsEqual = true;
    m_allSizeIsEqual = true;
    m_allFamilyIsEqual = true;
    m_allFontStyleIsEqual = true;

    if (block.isValid()) {
        QTextBlock::iterator it;

        for (it = block.begin(); !(it.atEnd()); ++it) {
            QTextFragment fragment = it.fragment();
            if (!fragment.isValid())
                continue;

            if (m_allColorIsEqual && fragment.charFormat().foreground().color() != m_pItem->getTextColor()) {
                m_allColorIsEqual = false;
            }

            if (m_allSizeIsEqual && fragment.charFormat().font().pointSize() != m_pItem->getFont().pointSize()) {
                m_allSizeIsEqual = false;
            }

            if (m_allFamilyIsEqual && fragment.charFormat().font().family() != m_pItem->getFont().family()) {
                m_allFamilyIsEqual = false;

                // 此处添加只要检测到被选中的字体不一样，字体的样式就不一样，会存在字体样式重叠交错的部分
                m_allFontStyleIsEqual = false;
            }

            if (m_allFontStyleIsEqual && fragment.charFormat().font().weight() != m_pItem->getFont().weight()) {
                m_allFontStyleIsEqual = false;
            }

            // 当所有的都不相同时跳出循环
            if (!m_allSizeIsEqual && !m_allColorIsEqual && !m_allFamilyIsEqual && !m_allFontStyleIsEqual) {
                return;
            }
        }
    }
}

void CTextEdit::setVisible(bool visible)
{

    QTextEdit::setVisible(visible);
    if (!visible) {
        QTextCursor cursor = this->textCursor();
        cursor.select(QTextCursor::Document);
        this->setTextCursor(cursor);
        if (nullptr != m_pItem->scene()) {
            auto curScene = static_cast<CDrawScene *>(m_pItem->scene());
            curScene->updateBlurItem(m_pItem);
        }

        //this->releaseKeyboard();
    } else {
        //this->grabKeyboard();
    }
}

void CTextEdit::setLastDocumentWidth(qreal width)
{
    m_widthF = width;
}

void CTextEdit::resizeDocument()
{
    if (this->document()->isEmpty()) {
        m_pItem->setFont(CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getTextFont());
        m_pItem->setTextColor(CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getTextColor());
    }

    if (m_pItem->getManResizeFlag() || this->document()->lineCount() > 1) {
        this->setLineWrapMode(WidgetWidth);
    }

    QSizeF size = this->document()->size();
    QRectF rect = m_pItem->rect();
    rect.setHeight(size.height());
    //rect.setWidth(size.width());

    //判断是否出界
    //QPointF bottomRight = rect.bottomRight();
    //QPointF bottomRightInScene = m_pItem->mapToScene(bottomRight);
    /*if (m_pItem->scene() != nullptr && !m_pItem->scene()->sceneRect().contains(bottomRightInScene)) {
        this->setLineWrapMode(WidgetWidth);
        this->document()->setTextWidth(m_widthF);
        size = this->document()->size();
        rect.setHeight(size.height());
        rect.setWidth(size.width());
    }*/

    if (m_pItem != nullptr) {
        m_pItem->setRect(rect);
    }

    m_widthF = rect.width();
}

bool CTextEdit::getAllTextColorIsEqual()
{
    return m_allColorIsEqual;
}

bool CTextEdit::getAllFontSizeIsEqual()
{
    return m_allSizeIsEqual;
}

bool CTextEdit::getAllFontFamilyIsEqual()
{
    return m_allFamilyIsEqual;
}

bool CTextEdit::getAllFontStyleIsEqual()
{
    return m_allFontStyleIsEqual;
}

void CTextEdit::setFontStyle(QFont ft)
{
    QTextCursor cur = this->textCursor();
    QTextBlock block = cur.block();
    if (block.isValid()) {
        QTextBlock::iterator it;
        for (it = block.begin(); !(it.atEnd()); ++it) {
            it.fragment().charFormat().setFont(ft);
        }
    }
}



