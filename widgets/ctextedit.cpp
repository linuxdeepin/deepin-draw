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


CTextEdit::CTextEdit(const QString &text, CGraphicsTextItem *item, QWidget *parent)
    : DTextEdit(text, parent)
    , m_pItem(item)
{
    connect(this, SIGNAL(textChanged()), this, SLOT(slot_textChanged()));
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
    QSizeF size = this->document()->size();
    QRectF rect = m_pItem->rect();
    if (size.height() > rect.size().height()) {
        rect.setSize(size);
        m_pItem->setRect(rect);
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


