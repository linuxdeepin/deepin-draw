#include "idrawtool.h"


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

void IDrawTool::keyPressEvent(QKeyEvent *event)
{
    if ( event->key() == Qt::Key_Shift) {
        m_bShiftKeyPress = true;
    } else if (event->key() == Qt::Key_Alt) {
        m_bAltKeyPress = true;
    } else {
        ;
    }
}

void IDrawTool::keyReleaseEvent(QKeyEvent *event)
{
    if ( event->key() == Qt::Key_Shift) {
        m_bShiftKeyPress = false;
    } else if (event->key() == Qt::Key_Alt) {
        m_bAltKeyPress = false;
    } else {
        ;
    }
}

EDrawToolMode IDrawTool::getDrawToolMode() const
{
    return m_mode;
}
