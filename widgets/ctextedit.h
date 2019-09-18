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
#ifndef CTEXTEDIT_H
#define CTEXTEDIT_H

#include <DTextEdit>

#include <DMenu>
#include <DGraphicsView>

DWIDGET_USE_NAMESPACE

class CTextEdit : public DTextEdit
{
    Q_OBJECT
public:
    explicit CTextEdit(const QString &text, QWidget *parent = nullptr);

    void setView(DGraphicsView *view);

    virtual void setVisible(bool visible) Q_DECL_OVERRIDE;

signals:

private slots:
    void setTopAlignment();
    void setRightAlignment();
    void setLeftAlignment();
    void setCenterAlignment();

protected:
    void contextMenuEvent(QContextMenuEvent *e) Q_DECL_OVERRIDE;

private:
    DMenu *m_menu;
    QAction *m_cutAction;
    QAction *m_copyAction;
    QAction *m_pasteAction;
    QAction *m_selectAllAction;
    QAction *m_leftAlignAct;
    QAction *m_topAlignAct;
    QAction *m_rightAlignAct;
    QAction *m_centerAlignAct;

    DGraphicsView *m_view;
};

#endif // CTEXTEDIT_H
