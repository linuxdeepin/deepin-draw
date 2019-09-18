/*
 * Copyright (C) 2019 ~ %YEAR% Deepin Technology Co., Ltd.
 *
 * Author:     WangXing
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
#include <DMenu>
#include <QAction>
#include <QDebug>
#include <QTextBlock>

CTextEdit::CTextEdit(const QString &text, CGraphicsTextItem *item, QWidget *parent)
    : DTextEdit(text, parent)
    , m_pItem(item)
{
    //初始化字体
    //this->mergeCurrentCharFormat();
    connect(this, SIGNAL(textChanged()), this, SLOT(slot_textChanged()));
//    connect(this, &QTextEdit::currentCharFormatChanged,
//            this, &CTextEdit::currentCharFormatChanged);

    connect(this, SIGNAL(selectionChanged()),
            this, SLOT(cursorPositionChanged()));
    //connect(this->document(), SIGNAL(contentsChanged), this, SLOT(slot_textChanged()));
}

//void CTextEdit::setTopAlignment()
//{
//    setAlignment(Qt::AlignJustify);
//}

//void CTextEdit::setRightAlignment()
//{
//    setAlignment(Qt::AlignRight);
//}

//void CTextEdit::setLeftAlignment()
//{
//    setAlignment(Qt::AlignLeft);
//}

//void CTextEdit::setCenterAlignment()
//{
//    setAlignment(Qt::AlignCenter);
//}

void CTextEdit::slot_textChanged()
{
    if (this->document()->isEmpty()) {
        m_pItem->setFont(CDrawParamSigleton::GetInstance()->getTextFont());
        m_pItem->setTextColor(CDrawParamSigleton::GetInstance()->getTextColor());
    }

    QSizeF size = this->document()->size();
    QRectF rect = m_pItem->rect();
    rect.setHeight(size.height());

    if (m_pItem != nullptr) {
        m_pItem->setRect(rect);
    }
//    if (size.height() > rect.size().height()) {
//        rect.setSize(size);
//        m_pItem->setRect(rect);
    //    }
}

void CTextEdit::cursorPositionChanged()
{
    if (this->document()->isEmpty()) {
        m_pItem->setFont(CDrawParamSigleton::GetInstance()->getTextFont());
        m_pItem->setTextColor(CDrawParamSigleton::GetInstance()->getTextColor());
    } else {
        QTextCursor cursor = this->textCursor();
        if (cursor.hasSelection()) {
            int startPos = cursor.selectionStart();
            int endPos = cursor.selectionEnd();
            qDebug() << "startpos = " << startPos << "endPos" << endPos;
            QTextBlock block = cursor.block();
            if (block.isValid()) {
                QTextBlock::iterator it;
                bool flag = true;
                QTextCharFormat chfFirst;
                QString fontFamily = "";
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
                            CDrawParamSigleton::GetInstance()->setSingleFontFlag(true);
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
                                qDebug() << text;
                                if (endPos == fragment.position()) {
                                    break;
                                }
                                if (fontFamily != chf.font().family()) {
                                    CDrawParamSigleton::GetInstance()->setSingleFontFlag(false);
                                    m_pItem->currentCharFormatChanged(chfFirst);
                                    break;
                                }

                            } else {
                                if (fontFamily != chf.font().family()) {
                                    CDrawParamSigleton::GetInstance()->setSingleFontFlag(false);
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
            CDrawParamSigleton::GetInstance()->setSingleFontFlag(true);
            m_pItem->currentCharFormatChanged(cursor.charFormat());
        }
    }
}



void CTextEdit::setVisible(bool visible)
{
    if (!visible) {
        QTextCursor cursor = this->textCursor();
        cursor.select(QTextCursor::Document);
        this->setTextCursor(cursor);
    }
    DTextEdit::setVisible(visible);
}

void CTextEdit::mousePressEvent(QMouseEvent *event)
{
    if (this->isVisible()) {
        this->clearFocus();
        this->setFocus();
        currentCharFormatChanged(currentCharFormat());
    }
    DTextEdit::mousePressEvent(event);
}

void CTextEdit::currentCharFormatChanged(const QTextCharFormat &format)
{
    /*if (!this->textCursor().hasSelection()) {
        CDrawParamSigleton::GetInstance()->setSingleFontFlag(true);
        m_pItem->currentCharFormatChanged(format);
    }*/
    qDebug() << "currentCharFormatChanged " << "selection" << this->textCursor().hasSelection()
             << "start pos" << this->textCursor().selectionStart()
             << "end pos" << this->textCursor().selectionEnd()
             << "pos" << this->textCursor().position()
             << endl;
}


