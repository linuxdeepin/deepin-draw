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
#include "cdrawtoolfactory.h"
#include "cselecttool.h"
#include "crecttool.h"
#include "cellipsetool.h"
#include "clinetool.h"
#include "ctriangletool.h"
#include "ctexttool.h"
#include "cpolygontool.h"
#include "cpolygonalstartool.h"
#include "cpentool.h"
#include "ccuttool.h"
#include "cmasicotool.h"
#include "cpicturetool.h"
#include "cviewmanagement.h"
#include "cgraphicsview.h"


#include <QObject>

#include <stdio.h>
#include <dlfcn.h>

CDrawToolFactory::CDrawToolsMap CDrawToolFactory::s_tools = CDrawToolFactory::CDrawToolsMap();
IDrawTool *CDrawToolFactory::Create(EDrawToolMode mode)
{
    IDrawTool *pTool = nullptr;

    switch (mode) {
    case selection:
        pTool = new CSelectTool();
        break;
    case line:
        pTool = new CLineTool();
        break;
    case rectangle:
        pTool = new CRectTool();
        break;
    case ellipse:
        pTool = new CEllipseTool();
        break;
    case triangle:
        pTool = new CTriangleTool();
        break;
    case text:
        pTool = new CTextTool();
        break;
    case polygon:
        pTool = new CPolygonTool();
        break;
    case polygonalStar:
        pTool = new CPolygonalStarTool();
        break;
    case pen:
        pTool = new CPenTool();
        break;
    case cut:
        pTool = new CCutTool();
        break;
    case blur:
        pTool = new /*CMasicoTool*/IBlurTool();
        break;
    case picture:
        pTool = new CPictureTool();
        break;
    default:
        break;
    }

    return pTool;
}

bool blocked = false;
void CDrawToolFactory::installTool(IDrawTool *tool)
{
    if (tool == nullptr) {
        qWarning() << "tool is null !!!!!!!!";
        return;
    }


    s_tools.insert(tool->getDrawToolMode(), tool);

    QObject::connect(tool->toolButton(), &QAbstractButton::toggled, tool->toolButton(), [ = ](bool checked) {
        if (checked) {
            setCurrentTool(tool);
        }
    });
}

IDrawTool *CDrawToolFactory::tool(int toolId)
{
    auto itfind = s_tools.find(toolId);
    if (itfind != s_tools.end()) {
        return itfind.value();
    }
    return nullptr;
}

IDrawTool *CDrawToolFactory::currentTool()
{
    foreach (auto it, s_tools) {
        if (it->status() == IDrawTool::EReady || it->status() == IDrawTool::EWorking) {
            return it;
        }
    }
    return nullptr;
}

bool CDrawToolFactory::setCurrentTool(int mode, bool force)
{
    auto pTool = tool(mode);
    if (pTool != nullptr) {
        return setCurrentTool(pTool, force);
    }
    return false;
}

bool CDrawToolFactory::setCurrentTool(IDrawTool *tool, bool force)
{
    if (blocked)
        return false;

    if (tool == nullptr)
        return false;

    bool ret = true;

    if (tool->status() == IDrawTool::EDisAbled) {
        return false;
    }

    auto current = currentTool();
    if (tool != current) {
        bool doChange = true;
        if (current != nullptr && current->status() == IDrawTool::EWorking) {
            if (force) {
                current->interrupt();
            } else {
                qWarning() << "can not active another tool when one tool is working!";
                doChange = false;
            }
        }
        if (doChange) {
            auto pView = CManageViewSigleton::GetInstance()->getCurView();
            if (pView->getDrawParam()->getCurrentDrawToolMode() != tool->getDrawToolMode())
                pView->getDrawParam()->setCurrentDrawToolMode(tool->getDrawToolMode());

            //blocked = true;

            if (current != nullptr && current->toolButton() != nullptr) {
                blocked = true;
                current->toolButton()->setChecked(false);
                blocked = false;
                current->changeStatusFlagTo(IDrawTool::EIdle);
            }

            if (tool->toolButton() != nullptr) {
                blocked = true;
                tool->toolButton()->setChecked(true);
                blocked = false;
                tool->changeStatusFlagTo(IDrawTool::EReady);
            }

            //blocked = false;
        }
        ret = doChange;
    }
    return ret;
}

CDrawToolFactory::CDrawToolsMap &CDrawToolFactory::allTools()
{
    return s_tools;
}

IDrawTool *CDrawToolFactory::loadToolPlugin(const QString &pluginPath)
{
    //plugins
    //手动加载指定位置的so动态库
    std::string string = pluginPath.toStdString();

    void *handle = dlopen(string.c_str(), RTLD_NOW);

    qWarning() << "load plugin handle = " << handle;
    if (handle == nullptr) {
        std::string errmsg = std::string(dlerror());
        qWarning() << "load plugin error = " << QString::fromStdString(errmsg);
        return nullptr;
    }

    typedef IDrawTool*(*Fun)() ;

    /*根据动态链接库操作句柄与符号，返回符号对应的地址*/
    Fun addres  = (Fun)dlsym(handle, "creatTool");

    if (addres == nullptr)
        return nullptr;

    IDrawTool *result = addres();

    //dlclose(handle);

    return result;
}
