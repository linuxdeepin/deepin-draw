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

#include "frame/cundocommands.h"

CManagerAttributeService *CManagerAttributeService::instance = nullptr;
CManagerAttributeService *CManagerAttributeService::getInstance()
{
    if(nullptr == instance) {
        instance = new CManagerAttributeService();
    }
    return instance;
}

void CManagerAttributeService::showSelectedCommonProperty(CDrawScene *scence, QList<CGraphicsItem *> items)
{
    if(scence!=nullptr) {
        m_currentScence = scence;
    }
}

void CManagerAttributeService::setItemsCommonPropertyValue(EDrawProperty property, QVariant value)
{

}

CManagerAttributeService::CManagerAttributeService()
{

}
