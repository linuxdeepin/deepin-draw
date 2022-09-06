// SPDX-FileCopyrightText: 2020 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef CDRAWTOOLFACTORY_H
#define CDRAWTOOLFACTORY_H

#include "globaldefine.h"
class IDrawTool;
/**
 * @brief The CDrawToolFactory class
 * 工具创建工厂类
 */
class CDrawToolFactory
{
public:
    using CDrawToolsMap = QMap<int, IDrawTool *>;
    /**
     * @brief Create 创建工具对象
     * @param mode 创建的工具类型
     * @return 创建的工具
     */
    static IDrawTool *Create(EDrawToolMode mode);

    static IDrawTool *loadToolPlugin(const QString &pluginPath);
    static void       installTool(IDrawTool *tool);

    static IDrawTool *tool(int toolId);

    static CDrawToolsMap &allTools();

private:
    static CDrawToolsMap     s_tools;
};

#endif // CDRAWTOOLFACTORY_H
