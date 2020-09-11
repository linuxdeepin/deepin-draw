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
#include "cdrawtoolmanagersigleton.h"

CDrawToolManagerSigleton *CDrawToolManagerSigleton::m_pInstance = nullptr;

CDrawToolManagerSigleton::CDrawToolManagerSigleton()
{
    m_hashDrawTool.clear();
}

CDrawToolManagerSigleton *CDrawToolManagerSigleton::GetInstance()
{
    if (m_pInstance == nullptr) {
        m_pInstance  = new CDrawToolManagerSigleton();
    }
    return m_pInstance;
}

void CDrawToolManagerSigleton::insertDrawTool(EDrawToolMode mode, IDrawTool *tool)
{
    m_hashDrawTool.insert(mode, tool);
}

IDrawTool *CDrawToolManagerSigleton::getDrawTool(EDrawToolMode mode) const
{
    return m_hashDrawTool[mode];
}
