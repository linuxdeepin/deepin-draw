#include "cdrawtoolfactory.h"
#include "cselecttool.h"
#include "crecttool.h"
#include "cellipsetool.h"

IDrawTool *CDrawToolFactory::Create(EDrawToolMode mode)
{
    IDrawTool *pTool = nullptr;

    switch (mode) {
    case selection:
        pTool = new CSelectTool();
        break;
    case line:
        break;
    case rectangle:
        pTool = new CRectTool();
        break;
    case ellipse:
        pTool = new CEllipseTool();
        break;
    default:
        break;

    }

    return pTool;
}
