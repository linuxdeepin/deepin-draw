#include "idrawtool.h"
#include "cdrawscene.h"
#include <QDebug>
#include <QKeyEvent>


IDrawTool::~IDrawTool()
{

}

IDrawTool::IDrawTool(EDrawToolMode mode)
    : m_bMousePress(false)
    , m_sPointPress(0, 0)
    , m_sPointRelease(0, 0)
    , m_bShiftKeyPress(false)
    , m_bAltKeyPress(false)
    , m_mode(mode)
{

}

EDrawToolMode IDrawTool::getDrawToolMode() const
{
    return m_mode;
}
