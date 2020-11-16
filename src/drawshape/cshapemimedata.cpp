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
#include "cshapemimedata.h"
#include "cgraphicsitem.h"
#include "cgraphicsrectitem.h"
#include "cgraphicsellipseitem.h"
#include "cgraphicslineitem.h"
#include "cgraphicstriangleitem.h"
#include "cgraphicspolygonitem.h"
#include "cgraphicspolygonalstaritem.h"
#include "cgraphicstextitem.h"
#include "cgraphicsmasicoitem.h"
#include "cgraphicspenitem.h"
#include "cpictureitem.h"

//CShapeMimeData::CShapeMimeData(QList<QGraphicsItem *> items)
//{
//    m_itemList.clear();
//    foreach (QGraphicsItem *item, items) {
//        CGraphicsItem *pBzItem = dynamic_cast<CGraphicsItem *>(item);
//        if (pBzItem != nullptr && pBzItem->isBzItem()) {
//            CGraphicsItem *pNew = pBzItem->creatSameItem();
//            if (pNew != nullptr) {
//                m_itemList.append(pNew);
//            }
//        }
//    }
//}

CShapeMimeData::CShapeMimeData(const CGroupBzItemsTreeInfo &itemsTreeInfo):
    QMimeData()
{
    treeInfo = itemsTreeInfo;
}
CShapeMimeData::~CShapeMimeData()
{
//    foreach (QGraphicsItem *item, m_itemList) {
//        delete item;
//        item = nullptr;
//    }
//    m_itemList.clear();
}

//QList<CGraphicsItem *> CShapeMimeData::itemList() const
//{
//    return m_itemList;
//}

//QList<CGraphicsItem *> CShapeMimeData::creatCopyItems() const
//{
//    QList<CGraphicsItem *> retList;
//    foreach (QGraphicsItem *item, m_itemList) {
//        CGraphicsItem *pBzItem = dynamic_cast<CGraphicsItem *>(item);
//        if (pBzItem != nullptr && pBzItem->isBzItem()) {
//            CGraphicsItem *pNew = pBzItem->creatSameItem();
//            if (pNew != nullptr) {
//                retList.append(pNew);
//            }
//        }
//    }
//    return retList;
//}

CGroupBzItemsTreeInfo CShapeMimeData::itemsTreeInfo()
{
    return treeInfo;
}
