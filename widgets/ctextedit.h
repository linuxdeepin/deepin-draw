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
//#include "cgraphicstextitem.h"
#include <DTextEdit>

#include <DMenu>
#include <DGraphicsView>

DWIDGET_USE_NAMESPACE

class CGraphicsTextItem;

class CTextEdit : public DTextEdit
{
    Q_OBJECT
public:
    explicit CTextEdit(const QString &text, CGraphicsTextItem *item, QWidget *parent = nullptr);


    virtual void setVisible(bool visible) Q_DECL_OVERRIDE;

protected:
    virtual void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;

public slots:
    void currentCharFormatChanged(const QTextCharFormat &format);
    void slot_textChanged();


private:
    CGraphicsTextItem *m_pItem;
};

#endif // CTEXTEDIT_H
