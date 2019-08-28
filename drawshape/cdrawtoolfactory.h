#ifndef CDRAWTOOLFACTORY_H
#define CDRAWTOOLFACTORY_H

#include "globaldefine.h"
class IDrawTool;
class CDrawToolFactory
{
public:
    static IDrawTool *Create(EDrawToolMode mode);
};

#endif // CDRAWTOOLFACTORY_H
