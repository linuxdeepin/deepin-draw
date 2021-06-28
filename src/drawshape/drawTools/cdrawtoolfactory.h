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

    static IDrawTool *currentTool();
    static bool       setCurrentTool(int toolId, bool force = false);
    static bool       setCurrentTool(IDrawTool *tool, bool force = false);

    static CDrawToolsMap &allTools();

private:
    static CDrawToolsMap     s_tools;
};

#endif // CDRAWTOOLFACTORY_H
