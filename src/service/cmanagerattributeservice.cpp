/*
* Copyright (C) 2019 ~ 2020 Deepin Technology Co., Ltd.
*
* Author: Zhang Hao<zhanghao@uniontech.com>
*
* Maintainer: Zhang Hao <zhanghao@uniontech.com>
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program. If not, see <http://www.gnu.org/licenses/>.
*/
#include "cmanagerattributeservice.h"
#include "drawTools/cdrawtoolmanagersigleton.h"
#include "bzItems/cgraphicslineitem.h"
#include "bzItems/cgraphicsrectitem.h"
#include "bzItems/cgraphicsellipseitem.h"
#include "bzItems/cgraphicspolygonitem.h"
#include "bzItems/cgraphicstriangleitem.h"
#include "bzItems/cgraphicstextitem.h"
#include "bzItems/cgraphicspolygonalstaritem.h"
#include "drawItems/cgraphicsitemselectedmgr.h"
#include "bzItems/cgraphicspenitem.h"
#include "drawshape/cdrawscene.h"
#include "bzItems/cgraphicslineitem.h"
#include "bzItems/cgraphicsmasicoitem.h"
#include "drawTools/ccuttool.h"

//#include "frame/cundocommands.h"
#include "frame/cviewmanagement.h"
#include "frame/cgraphicsview.h"

#include "widgets/ctextedit.h"

#include <QGraphicsItem>
#include <QDebug>

//升序排列用
static bool zValueSortASC(CGraphicsItem *info1, CGraphicsItem *info2)
{
    return info1->zValue() <= info2->zValue();
}

CManagerAttributeService *CManagerAttributeService::instance = nullptr;
CManagerAttributeService *CManagerAttributeService::getInstance()
{
    if (nullptr == instance) {
        instance = new CManagerAttributeService();
    }
    return instance;
}

void CManagerAttributeService::showSelectedCommonProperty(CDrawScene *scence,
                                                          QList<CGraphicsItem *> items,
                                                          bool write2Cache)
{
    Q_UNUSED(scence)
    Q_UNUSED(items)
    Q_UNUSED(write2Cache)
}

void CManagerAttributeService::refreshSelectedCommonProperty(bool write2Cache)
{
    updateCurrentScence();
    if (m_currentScence) {
        QList<CGraphicsItem *> allItems;
        if (m_currentScence->selectGroup()->items().size() > 1) {
            allItems = m_currentScence->selectGroup()->items();
        } else {
            QList<QGraphicsItem *> allSelectItems = m_currentScence->selectedItems();
            for (int i = allSelectItems.size() - 1; i >= 0; i--) {
                if (allSelectItems.at(i)->zValue() == 0.0) {
                    allSelectItems.removeAt(i);
                    continue;
                }
                if (allSelectItems[i]->type() <= QGraphicsItem::UserType || allSelectItems[i]->type() >= EGraphicUserType::MgrType) {
                    allSelectItems.removeAt(i);
                }
            }

            if (allSelectItems.size() >= 1) {
                CGraphicsItem *item = static_cast<CGraphicsItem *>(allSelectItems.at(0));
                if (item != nullptr) {
                    allItems.append(item);
                }
            }
        }
        this->showSelectedCommonProperty(m_currentScence, allItems, write2Cache);
    }
}

CManagerAttributeService::CManagerAttributeService()
{
    m_currentScence = nullptr;
}

void CManagerAttributeService::updateCurrentScence()
{
    m_currentScence = static_cast<CDrawScene *>(CManageViewSigleton::GetInstance()->getCurView()->scene());
}

void CManagerAttributeService::doCut()
{
    updateCurrentScence();
    EDrawToolMode currentMode = CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getCurrentDrawToolMode();
    ///区分裁剪
    if (cut == currentMode) {
        ECutAttributeType attributeType = CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getCutAttributeType();
        IDrawTool *pTool = CDrawToolManagerSigleton::GetInstance()->getDrawTool(cut);
        if (attributeType == ECutAttributeType::ButtonClickAttribute) {
            if (nullptr != pTool) {
                static_cast<CCutTool *>(pTool)->changeCutType(CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getCutType(), m_currentScence);
            }
        } else if (attributeType == ECutAttributeType::LineEditeAttribute) {
            if (nullptr != pTool) {
                static_cast<CCutTool *>(pTool)->changeCutSize(m_currentScence, CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getCutSize());
            }
        }
    }
}

//void CManagerAttributeService::setPictureRotateOrFlip(ERotationType type)
//{
//    updateCurrentScence();

//    QList<QGraphicsItem *> items = m_currentScence->selectedItems();
//    if (items.count() != 0) {
//        CGraphicsItem *item = static_cast<CGraphicsItem *>(items.first());

//        if (item != nullptr) {
//            //CItemRotationCommand *addCommand = nullptr;
//            //addCommand = new CItemRotationCommand(m_currentScence, item, type);
//            //CManageViewSigleton::GetInstance()->getCurView()->pushUndoStack(addCommand);
//        }
//    }
//}

//bool CManagerAttributeService::allPictureItem(CDrawScene *scence, QList<CGraphicsItem *> items)
//{
//    Q_UNUSED(scence)
//    bool isAllPictureItem = true;
//    if (items.size() >= 1) {
//        for (int i = 0; i < items.size(); i++) {
//            CGraphicsItem *item = items.at(i);
//            EGraphicUserType mode = static_cast<EGraphicUserType>(item->type());
//            if (mode != PictureType) {
//                isAllPictureItem = false;
//                break;
//            }
//        }
//    } else {
//        isAllPictureItem = false;
//    }

//    if (isAllPictureItem) {
//        if (m_currentScence->getItemsMgr()->getItems().size() > 1) {
//            emit signalIsAllPictureItem(!(m_currentScence->getItemsMgr()->mapRectToScene(m_currentScence->getItemsMgr()->boundingRect()) == m_currentScence->sceneRect())
//                                        , items.size() > 1 ? true : false);
//        } else {
//            QList<QGraphicsItem *> allSelectItems = m_currentScence->selectedItems();
//            for (int i = allSelectItems.size() - 1; i >= 0; i--) {
//                if (allSelectItems.at(i)->zValue() == 0.0) {
//                    allSelectItems.removeAt(i);
//                    continue;
//                }
//                if (allSelectItems[i]->type() <= QGraphicsItem::UserType || allSelectItems[i]->type() >= EGraphicUserType::MgrType) {
//                    allSelectItems.removeAt(i);
//                }
//            }

//            if (allSelectItems.size() >= 1) {
//                CGraphicsItem *item = static_cast<CGraphicsItem *>(allSelectItems.at(0));
//                if (item != nullptr) {
//                    emit signalIsAllPictureItem(!(item->mapRectToScene(item->boundingRect()) == m_currentScence->sceneRect())
//                                                , items.size() > 1 ? true : false);
//                }
//            }
//        }
//    }
//    return isAllPictureItem;
//}
