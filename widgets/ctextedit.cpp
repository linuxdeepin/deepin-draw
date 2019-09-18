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
#include <DMenu>
#include <QAction>
#include <QDebug>


CTextEdit::CTextEdit(const QString &text, QWidget *parent)
    : DTextEdit(text, parent)
{
    m_menu = new DMenu(this);

    m_cutAction = new QAction(tr("Cut"));
    m_copyAction = new QAction(tr("Copy"));
    m_pasteAction = new QAction(tr("Paste"));
    m_selectAllAction = new QAction(tr("Select All"));

    QAction *deleteAct = new QAction(tr("Delete"));
    deleteAct->setEnabled(false);
    QAction *undoAct = new QAction(tr("Undo"));
    undoAct->setEnabled(false);

    QAction *raiseLayerAct = new QAction(tr("Raise Layer"));
    raiseLayerAct->setEnabled(false);
    QAction *lowerLayerAct = new QAction(tr("Lower Layer"));
    lowerLayerAct->setEnabled(false);
    QAction *layerToTopAct = new QAction(tr("Layer to Top"));
    layerToTopAct->setEnabled(false);
    QAction *layerToBottomAct = new QAction(tr("Layer to Bottom"));
    layerToBottomAct->setEnabled(false);

    m_leftAlignAct = new QAction(tr("Left Alignment"));
    m_topAlignAct = new QAction(tr("Top Alignment"));
    m_rightAlignAct = new QAction(tr("Right Alignment" ));
    m_centerAlignAct = new QAction(tr("Center Alignment"));

    m_menu->addAction(m_cutAction);
    m_menu->addAction(m_copyAction);
    m_menu->addAction(m_pasteAction);
    m_menu->addAction(m_selectAllAction);
    m_menu->addSeparator();

    m_menu->addAction(deleteAct);
    m_menu->addAction(undoAct);
    m_menu->addSeparator();

    m_menu->addAction(raiseLayerAct);
    m_menu->addAction(lowerLayerAct);
    m_menu->addAction(layerToTopAct);
    m_menu->addAction(layerToBottomAct);



    m_menu->addAction(m_leftAlignAct);
    m_menu->addAction(m_topAlignAct);
    m_menu->addAction(m_rightAlignAct);
    m_menu->addAction(m_centerAlignAct);


    connect(m_cutAction, SIGNAL(triggered()), this, SLOT(cut()));
    connect(m_copyAction, SIGNAL(triggered()), this, SLOT(copy()));
    connect(m_pasteAction, SIGNAL(triggered()), this, SLOT(paste()));
    connect(m_selectAllAction, SIGNAL(triggered()), this, SLOT(selectAll()));


    connect(m_leftAlignAct, SIGNAL(triggered()), this, SLOT(setLeftAlignment()));
    connect(m_topAlignAct, SIGNAL(triggered()), this, SLOT(setTopAlignment()));
    connect(m_rightAlignAct, SIGNAL(triggered()), this, SLOT(setRightAlignment()));
    connect(m_centerAlignAct, SIGNAL(triggered()), this, SLOT(setCenterAlignment()));
}

void CTextEdit::setTopAlignment()
{
    setAlignment(Qt::AlignJustify);
}

void CTextEdit::setRightAlignment()
{
    setAlignment(Qt::AlignRight);
}

void CTextEdit::setLeftAlignment()
{
    setAlignment(Qt::AlignLeft);
}

void CTextEdit::setCenterAlignment()
{
    setAlignment(Qt::AlignCenter);
}


void CTextEdit::setView(DGraphicsView *view)
{
    m_view = view;

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

void CTextEdit::contextMenuEvent(QContextMenuEvent *e)
{
    Q_UNUSED(e)

    //获取右键菜单的显示位置，左边工具栏宽度为60，顶端参数配置栏高度为40，右键菜单长宽为94*513，第一次显示的时候为100*30.
    QPoint menuPos;
    int rx;
    int ry;
    //qDebug() << cursor().pos()  << m_view->pos() << endl;
    if (cursor().pos().rx() - 60 > m_view->width() - m_menu->width()) {
        rx = m_view->width() - m_menu->width() + 60;
    } else {
        rx = cursor().pos().rx();
    }
    int temp = 475;


    if (cursor().pos().ry() - 40 > m_view->height() - temp) {
        ry = m_view->height() - temp + 40;
    } else {
        ry = cursor().pos().ry();
    }
    menuPos = QPoint(rx, ry);


    m_menu->move(menuPos);
    // m_menu->move(cursor().pos());
    m_menu->show();
}
