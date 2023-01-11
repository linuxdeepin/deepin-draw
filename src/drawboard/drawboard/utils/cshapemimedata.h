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
#ifndef CSHAPEMIMEDATA_H
#define CSHAPEMIMEDATA_H

#include "sitemdata.h"
#include <QMimeData>

class PageItem;

class DRAWLIB_EXPORT DrawBoardMimeData : public QMimeData
{
    Q_OBJECT
public:
    explicit DrawBoardMimeData(const UnitList &u);
    ~DrawBoardMimeData();

    bool hasFormat(const QString &mimetype) const;

    UnitList unit()const;

private:
    UnitList ut;
};

#endif // CSHAPEMIMEDATA_H
