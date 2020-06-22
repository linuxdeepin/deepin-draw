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
    /**
     * @brief Create 创建工具对象
     * @param mode 创建的工具类型
     * @return 创建的工具
     */
    static IDrawTool *Create(EDrawToolMode mode);
};

#endif // CDRAWTOOLFACTORY_H
