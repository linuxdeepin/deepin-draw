#include "idrawtool.h"


IDrawTool::~IDrawTool()
{

}

IDrawTool::IDrawTool(EDrawToolMode mode)
    : m_bMousePress(false)
    , m_sPointPress(0, 0)
    , m_sPointRelease(0, 0)
    , m_mode(mode)
{

}

EDrawToolMode IDrawTool::getDrawToolMode() const
{
    return m_mode;
}
