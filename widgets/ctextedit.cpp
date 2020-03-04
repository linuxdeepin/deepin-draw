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
{
    //初始化字体
    //this->mergeCurrentCharFormat();
    connect(this, SIGNAL(textChanged()), this, SLOT(slot_textChanged()));
//    connect(this, &QTextEdit::currentCharFormatChanged,
//            this, &CTextEdit::currentCharFormatChanged);

    connect(this, SIGNAL(cursorPositionChanged()),
            this, SLOT(cursorPositionChanged()));

    this->setLineWrapMode(NoWrap);
    this->setFrameStyle(NoFrame);
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

    //更新字图元
    CDrawScene::GetInstance()->updateBlurItem(m_pItem);

    this->setFocus();

//    qDebug() << "this focus = " << this->hasFocus() << endl;
//    if (size.height() > rect.size().height()) {
//        rect.setSize(size);
//        m_pItem->setRect(rect);
    //    }
}

void CTextEdit::cursorPositionChanged()
{
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
                            CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setSingleFontFlag(true);
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
                                    CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setSingleFontFlag(false);
                                    m_pItem->currentCharFormatChanged(chfFirst);
                                    break;
                                }

                            } else {
                                if (fontFamily != chf.font().family()) {
                                    CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setSingleFontFlag(false);
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
            CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setSingleFontFlag(true);
            m_pItem->currentCharFormatChanged(cursor.charFormat());
        }

        CDrawScene::GetInstance()->updateBlurItem(m_pItem);
        //this->setFocus();
    }
}



void CTextEdit::setVisible(bool visible)
{

    QTextEdit::setVisible(visible);
    if (!visible) {
        QTextCursor cursor = this->textCursor();
        cursor.select(QTextCursor::Document);
        this->setTextCursor(cursor);
        CDrawScene::GetInstance()->updateBlurItem(m_pItem);

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

void CTextEdit::mousePressEvent(QMouseEvent *event)
{
    if (this->isVisible()) {
        this->clearFocus();
        this->setFocus();
        currentCharFormatChanged(currentCharFormat());
    }
    QTextEdit::mousePressEvent(event);
}

void CTextEdit::currentCharFormatChanged(const QTextCharFormat &format)
{
    /*if (!this->textCursor().hasSelection()) {
        CDrawParamSigleton::GetInstance()->setSingleFontFlag(true);
        m_pItem->currentCharFormatChanged(format);
    }*/
//    qDebug() << "currentCharFormatChanged " << "selection" << this->textCursor().hasSelection()
//             << "start pos" << this->textCursor().selectionStart()
//             << "end pos" << this->textCursor().selectionEnd()
//             << "pos" << this->textCursor().position()
//             << endl;
}


