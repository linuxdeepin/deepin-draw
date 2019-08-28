#ifndef CDRAWTOOLMANAGERSIGLETON_H
#define CDRAWTOOLMANAGERSIGLETON_H

#include "globaldefine.h"
#include <QHash>
class IDrawTool;
class CDrawToolManagerSigleton
{
private :
    static CDrawToolManagerSigleton *m_pInstance;
    CDrawToolManagerSigleton();

public :
    static CDrawToolManagerSigleton *GetInstance();

    void insertDrawTool(EDrawToolMode mode, IDrawTool *tool);
    IDrawTool *getDrawTool(EDrawToolMode mode) const;

private:
    QHash <EDrawToolMode, IDrawTool *> m_hashDrawTool;
};

#endif // CDRAWTOOLMANAGERSIGLETONH_H
