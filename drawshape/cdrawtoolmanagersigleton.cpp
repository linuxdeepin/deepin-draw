#include "cdrawtoolmanagersigleton.h"

CDrawToolManagerSigleton *CDrawToolManagerSigleton::m_pInstance = nullptr;

CDrawToolManagerSigleton::CDrawToolManagerSigleton()
{
    m_hashDrawTool.clear();
}

CDrawToolManagerSigleton *CDrawToolManagerSigleton::GetInstance()
{
    if (m_pInstance == nullptr) {
        m_pInstance  = new CDrawToolManagerSigleton();
    }
    return m_pInstance;
}

void CDrawToolManagerSigleton::insertDrawTool(EDrawToolMode mode, IDrawTool *tool)
{
    m_hashDrawTool.insert(mode, tool);
}

IDrawTool *CDrawToolManagerSigleton::getDrawTool(EDrawToolMode mode) const
{
    return m_hashDrawTool[mode];
}
