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
    connect(this, SIGNAL(textChanged()), this, SLOT(slot_textChanged()));

    connect(this, SIGNAL(cursorPositionChanged()),
            this, SLOT(cursorPositionChanged()));

    this->setLineWrapMode(NoWrap);
    this->setFrameStyle(NoFrame);
}

CTextEdit::~CTextEdit()
{
    m_pItem = nullptr;
}

void CTextEdit::slot_textChanged()
{
    if (m_pItem->getManResizeFlag() || this->document()->lineCount() > 1) {
        this->setLineWrapMode(WidgetWidth);
    }

    QSizeF size = this->document()->size();
    QRectF rect = m_pItem->rect();
    rect.setHeight(size.height());
    rect.setWidth(size.width());

    //判断是否出界
//    QPointF bottomRight = rect.bottomRight();
//    QPointF bottomRightInScene = m_pItem->mapToScene(bottomRight);
//    if (m_pItem->scene() != nullptr && !m_pItem->scene()->sceneRect().contains(bottomRightInScene)) {
//        this->setLineWrapMode(WidgetWidth);
//        this->document()->setTextWidth(m_widthF);
//        size = this->document()->size();
//        rect.setHeight(size.height());
//        rect.setWidth(size.width());
//    }

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

void CTextEdit::checkTextProperty(QTextBlock block)
{
    if (block.isValid()) {
        QTextBlock::iterator it = block.begin();
        if (it == block.end()) {
            return;
        }

        QTextFragment first_fragment = it.fragment();
        if (!first_fragment.isValid()) {
            m_selectedColor = QColor();
            m_selectedSize = -1;
            m_selectedFamily.clear();
            m_selectedFontStyle.clear();
            m_selectedFontWeight = -1;
            m_selectedColorAlpha = -1;
            return;
        } else {
            m_selectedColor = first_fragment.charFormat().foreground().color();
            m_selectedSize = first_fragment.charFormat().font().pointSize();
            m_selectedFamily = first_fragment.charFormat().font().family();
            m_selectedFontStyle = first_fragment.charFormat().font().styleName();
            m_selectedColorAlpha = first_fragment.charFormat().foreground().color().alpha();
            m_selectedFontWeight = first_fragment.charFormat().font().weight();
        }

        for (; !it.atEnd(); ++it) {

            QTextFragment fragment = it.fragment();

            if (!fragment.isValid())
                continue;

            if (m_selectedColor.isValid() && fragment.charFormat().foreground().color() != first_fragment.charFormat().foreground().color()) {
                m_selectedColor = QColor();
            }

            if (m_selectedSize >= 0 && fragment.charFormat().font().pointSize() != first_fragment.charFormat().font().pointSize()) {
                m_selectedSize = -1;
            }

            if (!m_selectedFamily.isEmpty() && fragment.charFormat().font().family() != first_fragment.charFormat().font().family()) {
                m_selectedFamily.clear();
            }

            // 注意：这里Qt版本是5.11.3,只能用这种方式去匹配，不可以用charFormat().font().weight()
            if (m_selectedFontWeight >= 0 && fragment.charFormat().fontWeight() != first_fragment.charFormat().fontWeight()) {
                m_selectedFontWeight = -1;
            }

            if (m_selectedColorAlpha >= 0 && fragment.charFormat().foreground().color().alpha() != first_fragment.charFormat().foreground().color().alpha()) {
                m_selectedColorAlpha = -1;
            }

            // 当所有的都不相同时跳出循环
            if (!m_selectedSize && m_selectedColor.isValid() && m_selectedFamily.isEmpty()
                    && m_selectedFontStyle.isEmpty() && !m_selectedColorAlpha) {
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
        if (m_pItem != nullptr && nullptr != m_pItem->scene()) {
            auto curScene = static_cast<CDrawScene *>(m_pItem->scene());
            curScene->updateBlurItem(m_pItem);
        }
    }
}

void CTextEdit::setLastDocumentWidth(qreal width)
{
    m_widthF = width;
}

void CTextEdit::resizeDocument()
{
    if (m_pItem->getManResizeFlag() || this->document()->lineCount() > 1) {
        this->setLineWrapMode(WidgetWidth);
    }

    QSizeF size = this->document()->size();
    QRectF rect = m_pItem->rect();
    rect.setHeight(size.height());

    if (m_pItem != nullptr) {
        m_pItem->setRect(rect);
    }

    m_widthF = rect.width();
}

QColor CTextEdit::getSelectedTextColor()
{
    return m_selectedColor;
}

int CTextEdit::getSelectedFontSize()
{
    return m_selectedSize;
}

QString CTextEdit::getSelectedFontFamily()
{
    return m_selectedFamily;
}

QString CTextEdit::getSelectedFontStyle()
{
    /* 注意：5.11.3版本中 QTextCharFormat 不支持 setFontStyleName 接口
     * 只有在5.13之后才支持，同时无法直接设置font的样式然后修改字体字重
     * 后续Qt版本升级后可以查看相关文档使用 setFontStyleName 接口
    */
    //    QFont::Thin(Regular)    0   QFont::ExtraLight 12  QFont::Light 25
    //    QFont::Normal  50  QFont::Medium     57  QFont::DemiBold 63
    //    QFont::Bold    75  QFont::ExtraBold  81  QFont::Black 87
    switch (m_selectedFontWeight) {
    case 0: {
        m_selectedFontStyle = QObject::tr("Regular");
        break;
    }
    case 12: {
        m_selectedFontStyle = QObject::tr("ExtraLight");
        break;
    }
    case 25: {
        m_selectedFontStyle = QObject::tr("Light");
        break;
    }
    case 50: {
        m_selectedFontStyle = QObject::tr("Normal");
        break;
    }
    case 57: {
        m_selectedFontStyle = QObject::tr("Medium");
        break;
    }
    case 63: {
        m_selectedFontStyle = QObject::tr("DemiBold");
        break;
    }
    case 75: {
        m_selectedFontStyle = QObject::tr("Bold");
        break;
    }
    case 81: {
        m_selectedFontStyle = QObject::tr("ExtraBold");
        break;
    }
    case 87: {
        m_selectedFontStyle = QObject::tr("Black");
        break;
    }
    }
    return m_selectedFontStyle;
}

int CTextEdit::getSelectedFontWeight()
{
    return m_selectedFontWeight;
}

int CTextEdit::getSelectedTextColorAlpha()
{
    return m_selectedColorAlpha;
}



