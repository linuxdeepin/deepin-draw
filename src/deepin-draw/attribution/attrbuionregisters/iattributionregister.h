/*
 * Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
 *
 * Author:     Tan Lang <tanlang@uniontech.com>
 *
 * Maintainer: Tan Lang <tanlang@uniontech.com>
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
#ifndef IATRRIBUTIONREGISTER_H
#define IATRRIBUTIONREGISTER_H

#include "drawboard.h"
#include "attributemanager.h"

#include <QObject>

class IAttributionRegister : public QObject
{
    Q_OBJECT
public:
    explicit IAttributionRegister(DrawBoard *drawBoard = nullptr, QObject *parent = nullptr):  QObject(parent), m_drawBoard(drawBoard) {}
    virtual void registe() = 0;

    void setDrawBoard(DrawBoard  *drawBoard)
    {
        m_drawBoard = drawBoard;
    }

    DrawBoard *drawBoard()
    {
        return m_drawBoard;
    }

private:
    DrawBoard     *m_drawBoard;
};

#endif // IATRRIBUTIONREGISTER_H
