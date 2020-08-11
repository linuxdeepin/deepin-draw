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
#include "cgraphicsproxywidget.h"
#include <QTextEdit>
#include <QDebug>
CGraphicsProxyWidget::CGraphicsProxyWidget(QGraphicsItem *parent, Qt::WindowFlags wFlags)
    : QGraphicsProxyWidget(parent, wFlags)
{

}

void CGraphicsProxyWidget::focusOutEvent(QFocusEvent *event)
{
    //清空选中状态
    //static_cast<QTextEdit *>(this->widget())->setTextCursor(QTextCursor());
    //this->hide();

//    QTextEdit *textEdit = qobject_cast<QTextEdit *>(this->widget());
//    if (textEdit != nullptr) {
//        QTextCursor cursor = textEdit->textCursor();

//        cursor.movePosition(QTextCursor::End);

//        textEdit->setTextCursor(cursor);
//    }
    qDebug() << "-----focusOutEvent ----- ";
    QGraphicsProxyWidget::focusOutEvent(event);
}
