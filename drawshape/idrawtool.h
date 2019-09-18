/*
 * Copyright (C) 2019 ~ %YEAR% Deepin Technology Co., Ltd.
 *
 * Author:     WangXing
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef CDRAWTOOL_H
#define CDRAWTOOL_H

#include "globaldefine.h"
#include <QList>

class QGraphicsSceneMouseEvent;
class CDrawScene;

class IDrawTool
{
public:
    IDrawTool(EDrawToolMode mode);
    virtual ~IDrawTool() = 0;
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene) = 0;
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene) = 0;
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene) = 0;

    EDrawToolMode getDrawToolMode() const;

protected:
    bool m_bMousePress;
    QPointF m_sPointPress;
    QPointF m_sLastPress;
    QPointF m_sPointRelease;

    bool m_bShiftKeyPress;
    bool m_bAltKeyPress;


private:
    EDrawToolMode m_mode;

};

#endif // CDRAWTOOL_H
