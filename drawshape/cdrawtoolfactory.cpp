#include "cdrawtoolfactory.h"
#include "cselecttool.h"
#include "crecttool.h"

IDrawTool *CDrawToolFactory::Create(EDrawToolMode mode)
{
    IDrawTool *pTool = nullptr;

    switch (mode) {
    case selection:
        pTool = new CSelectTool();
        break;
    case rotation:
        break;
    case line:
        break;
    case rectangle:
        pTool = new CRectTool();
        break;
    case roundrect:
        break;
    case ellipse:
        break;
    case poly:
        break;
    default:
        break;

    }
}
