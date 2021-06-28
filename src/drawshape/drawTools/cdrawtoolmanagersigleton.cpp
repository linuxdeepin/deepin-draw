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
#include "cdrawtoolmanagersigleton.h"
#include "idrawtool.h"
#include <QDebug>
#include "cdrawtoolfactory.h"

CDrawToolManagerSigleton *CDrawToolManagerSigleton::m_pInstance = nullptr;

CDrawToolManagerSigleton::CDrawToolManagerSigleton()
{
    //m_hashDrawTool.clear();
}

void CDrawToolManagerSigleton::toolManagerDeconstruction()
{
//    foreach (IDrawTool *tool, m_hashDrawTool.values()) {
//        delete tool;
//        tool = nullptr;
//    }
//    m_hashDrawTool.clear();
//    if (m_pInstance) {
//        delete m_pInstance;
//        m_pInstance = nullptr;
//    }
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
    //m_hashDrawTool.insert(mode, tool);
    CDrawToolFactory::installTool(tool);
}

IDrawTool *CDrawToolManagerSigleton::getDrawTool(EDrawToolMode mode) const
{
    //return m_hashDrawTool[mode];
    return CDrawToolFactory::tool(mode);
}

