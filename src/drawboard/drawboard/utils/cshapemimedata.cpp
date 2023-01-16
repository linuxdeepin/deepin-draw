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
#include "cshapemimedata.h"

//CShapeMimeData::CShapeMimeData(const UnitTree_Comp &itemsTreeInfo):
//    QMimeData(), treeInfo(itemsTreeInfo)
//{
//}
DrawBoardMimeData::DrawBoardMimeData(const UnitList &u): QMimeData(), ut(u)
{

}

DrawBoardMimeData::~DrawBoardMimeData()
{

}

bool DrawBoardMimeData::hasFormat(const QString &mimetype) const
{
    if (mimetype == "pageItems") {
        return true;
    }
    return false;
}

UnitList DrawBoardMimeData::unit() const
{
    return ut;
}
