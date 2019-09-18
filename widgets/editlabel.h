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
#ifndef EDITLABEL_H
#define EDITLABEL_H

#include <DLabel>
#include <DLineEdit>

DWIDGET_USE_NAMESPACE

class EditLabel : public DLabel
{
    Q_OBJECT
public:
    EditLabel(DWidget *parent = 0);
    ~EditLabel();

    void setTitle(QString title);
    void setEditText(QString text);
    void setEditWidth(int width);
    void setTitleSpacing(int spacing);
    QString editText();

signals:
    void editTextChanged(QString text);

private:
    DLabel *m_titleLabel;
    DLineEdit *m_edit;
    int m_titleSpacing;
};

#endif // EDITLABEL_H
