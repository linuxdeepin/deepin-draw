// SPDX-FileCopyrightText: 2020 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef CDRAWTOOLMANAGERSIGLETON_H
#define CDRAWTOOLMANAGERSIGLETON_H

#include "globaldefine.h"
#include <QMap>
class IDrawTool;
class CDrawToolManagerSigleton
{
private :
    static CDrawToolManagerSigleton *m_pInstance;
    CDrawToolManagerSigleton();

public :
    static CDrawToolManagerSigleton *GetInstance();
    //void toolManagerDeconstruction();
    //void insertDrawTool(EDrawToolMode mode, IDrawTool *tool);
    IDrawTool *getDrawTool(EDrawToolMode mode) const;

    int toolCount();


private:
    //QMap <EDrawToolMode, IDrawTool *> m_hashDrawTool;
};

#endif // CDRAWTOOLMANAGERSIGLETONH_H
