/*
 * Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
 *
 * Author:     Ji XiangLong <jixianglong@uniontech.com>
 *
 * Maintainer: WangYu <wangyu@uniontech.com>
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
#include "cgraphicstextitem.h"
#include "cgraphicsitem.h"
#include "cgraphicsview.h"
#include "cdrawscene.h"
#include "ctextedit.h"
#include "ccolorpickwidget.h"
#include "colorpanel.h"
#include "cattributemanagerwgt.h"

#include <QComboBox>
#include <QTextEdit>
#include <QLineEdit>
#include <QDebug>
#include <QApplication>
#include "application.h"
#include "cundoredocommand.h"
CGraphicsProxyWidget::CGraphicsProxyWidget(QGraphicsItem *parent, Qt::WindowFlags wFlags)
    : QGraphicsProxyWidget(parent, wFlags)
{
    setCursor(QCursor(Qt::IBeamCursor));
}

CGraphicsItem *CGraphicsProxyWidget::parentDrawItem()
{
    return dynamic_cast<CGraphicsItem *>(parentItem());
}

void CGraphicsProxyWidget::focusInEvent(QFocusEvent *event)
{
    Q_UNUSED(event)
    //如果不具有内容那么不设置焦点
    if (this->flags() & ItemHasNoContents) {
        scene()->setFocusItem(nullptr);
    }
}

void CGraphicsProxyWidget::focusOutEvent(QFocusEvent *event)
{
    QGuiApplication::inputMethod()->reset();

    if (parentDrawItem() && parentDrawItem()->drawScene()) {
        if (qobject_cast<CTextEdit *>(widget()) != nullptr) {
            CTextEdit *pTextEditor = qobject_cast<CTextEdit *>(widget());
            pTextEditor->selectAll();
        }
        setFlag(ItemHasNoContents, true);
        parentDrawItem()->drawScene()->notSelectItem(parentDrawItem());
    }

    return QGraphicsProxyWidget::focusOutEvent(event);
}

void CGraphicsProxyWidget::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QGraphicsProxyWidget::paint(painter, option, widget);
}

QVariant CGraphicsProxyWidget::itemChange(QGraphicsItem::GraphicsItemChange doChange, const QVariant &value)
{
    auto result = QGraphicsProxyWidget::itemChange(doChange, value);
    if (doChange == ItemFlagsHaveChanged) {
        auto parentDrawItem = this->parentDrawItem();
        if (parentDrawItem != nullptr && parentDrawItem->drawScene() != nullptr) {
            static QSizeF s_size = QSizeF(0, 0);
            int flags = value.toInt();
            bool show = !(flags & ItemHasNoContents);
            //qWarning() << QString("go %1 proxy widget (will record parent item data for %2) = ").arg(show ? "show" : "hide").arg(show ? "undo" : "redo") << show;
            if (show) {
                //1.显示出来证明开始进入编辑状态,进入编辑状态前收集当前文本的信息以用于外部撤销
                parentDrawItem->drawScene()->recordItemsInfoToCmd(QList<CGraphicsItem *>() << parentDrawItem, UndoVar);
                s_size = parentDrawItem->rect().size();
            } else {
                //2.隐藏内容意味着退出编辑,那么这个时候判断内部文本是否可撤销,如果可撤销证明修改过,那么就收集还原信息
                bool pushToStash = false;
                if (qobject_cast<CTextEdit *>(widget()) != nullptr) {
                    CTextEdit *pTextEditor = qobject_cast<CTextEdit *>(widget());
                    pushToStash = pTextEditor->document()->isUndoAvailable();
                }
                if (!pushToStash) {
                    pushToStash = (s_size != parentDrawItem->rect().size());
                }
                if (pushToStash) {
                    parentDrawItem->drawScene()->recordItemsInfoToCmd(QList<CGraphicsItem *>() << parentDrawItem, RedoVar);
                }
                parentDrawItem->drawScene()->finishRecord();
            }
        }
    }
    return result;
}
