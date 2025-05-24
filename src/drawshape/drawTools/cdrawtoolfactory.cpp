// SPDX-FileCopyrightText: 2020 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

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
#include "cerasertool.h"

#include <QObject>
#include <QDebug>

#include <stdio.h>
#include <dlfcn.h>

CDrawToolFactory::CDrawToolsMap CDrawToolFactory::s_tools = CDrawToolFactory::CDrawToolsMap();

IDrawTool *CDrawToolFactory::Create(EDrawToolMode mode)
{
    qDebug() << "Creating draw tool for mode:" << mode;
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
    case eraser:
        pTool = new CEraserTool();
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
        qDebug() << "Unknown tool mode:" << mode;
        break;
    }

    if (pTool != nullptr) {
        qDebug() << "Successfully created tool for mode:" << mode;
    } else {
        qDebug() << "Failed to create tool for mode:" << mode;
    }

    return pTool;
}

//bool blocked = false;
void CDrawToolFactory::installTool(IDrawTool *tool)
{
    if (tool == nullptr) {
        qWarning() << "Cannot install null tool";
        return;
    }

    qDebug() << "Installing tool with mode:" << tool->getDrawToolMode();
    s_tools.insert(tool->getDrawToolMode(), tool);

//    QObject::connect(tool->toolButton(), &QAbstractButton::toggled, tool->toolButton(), [ = ](bool checked) {
//        if (checked) {
//            setCurrentTool(tool);
//        }
//    });
}

IDrawTool *CDrawToolFactory::tool(int toolId)
{
    qDebug() << "Looking up tool with ID:" << toolId;
    auto itfind = s_tools.find(toolId);
    if (itfind != s_tools.end()) {
        qDebug() << "Found tool for ID:" << toolId;
        return itfind.value();
    }
    qDebug() << "No tool found for ID:" << toolId;
    return nullptr;
}

CDrawToolFactory::CDrawToolsMap &CDrawToolFactory::allTools()
{
    qDebug() << "Getting all tools, count:" << s_tools.size();
    return s_tools;
}

IDrawTool *CDrawToolFactory::loadToolPlugin(const QString &pluginPath)
{
    //plugins
    //手动加载指定位置的so动态库
    qDebug() << "Loading tool plugin from path:" << pluginPath;
    
    std::string string = pluginPath.toStdString();
    void *handle = dlopen(string.c_str(), RTLD_NOW);

    if (handle == nullptr) {
        std::string errmsg = std::string(dlerror());
        qWarning() << "Failed to load plugin:" << QString::fromStdString(errmsg);
        return nullptr;
    }
    qDebug() << "Successfully loaded plugin handle:" << handle;

    /*根据动态链接库操作句柄与符号，返回符号对应的地址*/
    typedef IDrawTool*(*Fun)();
    Fun addres = (Fun)dlsym(handle, "creatTool");

    if (addres == nullptr) {
        qWarning() << "Failed to find 'creatTool' symbol in plugin";
        return nullptr;
    }

    IDrawTool *result = addres();

    //dlclose(handle);

    return result;
}
