#include "ctextedit.h"
#include <DMenu>
#include <QAction>


CTextEdit::CTextEdit(const QString &text, QWidget *parent)
    : DTextEdit(text, parent)
{
    m_menu = new DMenu(this);

    m_cutAction = new QAction(tr("Cut"));
    m_copyAction = new QAction(tr("Copy"));
    m_pasteAction = new QAction(tr("Paste"));
    m_selectAllAction = new QAction(tr("Select All"));

    m_leftAlignAct = new QAction(tr("Left Alignment"));
    m_topAlignAct = new QAction(tr("Top Alignment"));
    m_rightAlignAct = new QAction(tr("Right Alignment" ));
    m_centerAlignAct = new QAction(tr("Center Alignment"));

    m_menu->addAction(m_cutAction);
    m_menu->addAction(m_copyAction);
    m_menu->addAction(m_pasteAction);
    m_menu->addAction(m_selectAllAction);
    m_menu->addSeparator();
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


void CTextEdit::contextMenuEvent(QContextMenuEvent *e)
{
    Q_UNUSED(e)

    m_menu->move(cursor().pos());
    m_menu->show();
}
